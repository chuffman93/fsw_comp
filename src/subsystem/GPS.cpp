/*
 * GPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#include "subsystem/GPS.h"

#include <string.h>
#include "util/TimeKeeper.h"

#define CRC32_POLYNOMIAL	0xEDB88320L

std::ostream &operator<<(std::ostream &output, const GPSPositionTime &g ) {
	output << "{x:" << g.posX << ", y:" << g.posY << ", z:" << g.posZ
			<< ", vx:" << g.velX << ", vy:" << g.velY << ", vz:" << g.velZ
			<< ", w:" << g.GPSWeek << ", s:" << g.GPSSec << "}";
	return output;
}

GPS::GPS(NMEAInterface& nm, SubPowerInterface& pow):nm(nm), pow(pow){
	tags += LogTag("Name", "GPS");
	solSuccess = false;
	power = false;
	isLocked = false;
	timeout = 0;
	timein = 0;
	lockTries = 0;
}


GPS::~GPS(){}

void GPS::initialize(){
	pow.configDelay(0, 1000);
	powerOff();
	powerOn();
	handleConfig();
}

void GPS::handleMode(FSWMode transition){}

//! handles the configs for GPS
void GPS::handleConfig(){
	LockGuard l(lock);
	if(FileManager::checkExistance(GPS_CONFIG)){
		std::vector<uint8_t> buff = FileManager::readFromFile(GPS_CONFIG);
		if(buff.size() != CONFIG_GPS_SIZE){
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect Size for config";
			return;
		}
		ByteStream bs(buff);
		bs >> timeout >> timein;
		Logger::Stream(LEVEL_INFO,tags) << "Setting timeout to " << timeout/60 << " mins and timein to " << timein/3600 << " hrs";

	}else{
		Logger::Stream(LEVEL_WARN,tags) << "GPS Config file does not exist";
	}
}

//! handles updating configs if needed
void GPS::updateConfig(){
	LockGuard l(lock);
	if(FileManager::checkExistance(GPS_CONFIG_UP)){
		std::vector<uint8_t> buff = FileManager::readFromFile(GPS_CONFIG_UP);
		if(buff.size() != CONFIG_GPS_SIZE){
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect Size for config";
			return;
		}
		ByteStream bs(buff);
		bs >> timeout >> timein;
		Logger::Stream(LEVEL_INFO,tags) << "Updating timeout to " << timeout/60 << " mins and timein to " << timein/3600 << " hrs";
		FileManager::moveFile(GPS_CONFIG_UP,GPS_CONFIG);
	}else{
		Logger::Stream(LEVEL_WARN,tags) << "There are no GPS config updates";
	}
}

/*!
 * Handles sending opcodes
 * \param opcode to be sent
 * \param buffer to be sent if needed
 */
ACPPacket GPS::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	assert(false);
	return ACPPacket(ACS_SYNC, 0);
}

// TODO: Will this be needed?
void GPS::getHealthStatus(){
	LockGuard l(lock);
	Logger::log(LEVEL_WARN, tags, "GPS Health and Status isn't implemented yet!");
}

//! Gets the Propagated ECI coordinates from the orbital elements
GPSPositionTime GPS::getBestXYZI(){
	LockGuard l(lock);
	float eciPos[3];
	float eciVel[3];
	int64_t currTime = getCurrentTime()/1000.0;
	float propTime = currTime - lastLock.sysTime;
	Logger::Stream(LEVEL_DEBUG,tags) << "PropTime: " << propTime;
	propagatePositionVelocity(lastLock.elements, propTime, eciPos, eciVel);

	GPSPositionTime pt;
	pt.posX = eciPos[0];
	pt.posY = eciPos[1];
	pt.posZ = eciPos[2];
	pt.velX = eciVel[0];
	pt.velY = eciVel[1];
	pt.velZ = eciVel[2];
	pt.GPSSec = lastLock.GPSSec;
	pt.GPSWeek = lastLock.GPSWeek;
	Logger::Stream(LEVEL_DEBUG) << lastLock.elements.a << lastLock.elements.e << lastLock.elements.i << lastLock.elements.Omega << lastLock.elements.omega << lastLock.elements.anom;
	incrementGPSTime(pt.GPSWeek, pt.GPSSec, propTime);
	return pt;
}

/*!
 * Waits to get new GPS coordinates then parses them
 * Takes the ECEF coords and tranforms them into ECI
 * Then takes the ECI coordinates and chages them into orbital parameters
 */
void GPS::fetchNewGPS(){
	LockGuard l(lock);
	std::string data = nm.getString();
	if(data == ""){
		Logger::Stream(LEVEL_DEBUG,tags) << "Nothing was read";
		return; //Nothing was read.
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "Reading in: " << data;

	//Everything below this line is from old FSW
	GPSPositionTime tempData = {0,0,0,0,0,0,0,0};
	char buffer[1024];
	char * buffPtr0 = strcpy(buffer,data.c_str());
	char * buffPtr1 = strcpy(buffer,data.c_str());
	solSuccess = true;

	bool containsDelimiter = false;

	while ((buffPtr0 - data.c_str() != 350) && (*buffPtr0 != '\0')) {
		if (*buffPtr0++ == '*') {
			containsDelimiter = true;
			break;
		}
	}

	if (!containsDelimiter) {
		Logger::log(LEVEL_WARN, tags, "String doesn't contain '*' ");
		solSuccess = false;
		return;
	}

	// split into message and crc
	char * saveIndex0;
	char * message = strtok_r(buffPtr1, "*", &saveIndex0);
	char * crcStr = strtok_r(NULL, "*", &saveIndex0);

	char * tailPtr;
	uint32_t crc = strtoul(crcStr, &tailPtr, 16);

	// validate crc
	if (crc != CalculateCRC_GPS(message)) {
		Logger::log(LEVEL_WARN, tags, "CRC Failed");
		//return false; TODO: when working with the simulator, we need to ignore the CRC
	}

	// parse the message into header and log
	char * saveIndex1;
	char * header = strtok_r(message, ";", &saveIndex1);
	char * log = strtok_r(NULL, ";", &saveIndex1);

	// parse the message header
	char * saveIndex2;
	char * type = strtok_r(header, ",", &saveIndex2);

	if (strcmp("#BESTXYZA", type) != 0) {
		Logger::Stream(LEVEL_WARN, tags) << "Wrong message. Expected #BESTXYZA got:\"" << type << "\"";
		solSuccess = false;
		return;
	}

	char * token;
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) port
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) sequence num
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) idle time

	if (strcmp("FINESTEERING", strtok_r(NULL, ",", &saveIndex2)) != 0) {
		Logger::log(LEVEL_DEBUG, tags, "Fine steering not reached!");
		solSuccess = false;
		return;
	}

	//TODO: Check for finesteering
	tempData.GPSWeek = (int32_t) strtoul(strtok_r(NULL, ",", &saveIndex2), NULL, 10);

	tempData.GPSSec = strtof(strtok_r(NULL, ",", &saveIndex2), NULL);
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) receiver status
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) port
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) reserved for vendor
	token = strtok_r(NULL, ",", &saveIndex2); // (UNUSED) software build

	// parse the position log part of the message
	char * saveIndex3;
	if (strcmp("SOL_COMPUTED", strtok_r(log, ",", &saveIndex3)) != 0) {
		Logger::log(LEVEL_DEBUG, tags, "No position solution computed!");
		solSuccess = false;
		return;
	} else {
		Logger::log(LEVEL_DEBUG, tags, "Valid position solution computed!");
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) position type
		tempData.posX = strtod(strtok_r(NULL, ",", &saveIndex3), NULL) / 1000.0; // convert from m to km
		tempData.posY = strtod(strtok_r(NULL, ",", &saveIndex3), NULL) / 1000.0; // convert from m to km
		tempData.posZ = strtod(strtok_r(NULL, ",", &saveIndex3), NULL) / 1000.0; // convert from m to km
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) std dev x
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) std dev y
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) std dev z
	}

	// parse the velocity log part of the message
	if (strcmp("SOL_COMPUTED", strtok_r(NULL, ",", &saveIndex3)) != 0) {
		Logger::log(LEVEL_DEBUG, tags, "No velocity solution computed!");
		solSuccess = false;
		return;
	} else {
		Logger::log(LEVEL_DEBUG, tags, "Velocity solution computed!");
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) velocity type
		tempData.velX = strtod(strtok_r(NULL, ",", &saveIndex3), NULL) / 1000.0; // convert from m to km
		tempData.velY = strtod(strtok_r(NULL, ",", &saveIndex3), NULL) / 1000.0; // convert from m to km
		tempData.velZ = strtod(strtok_r(NULL, ",", &saveIndex3), NULL) / 1000.0; // convert from m to km
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) std dev x
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) std dev y
		token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) std dev z
	}

	// parse the rest of the log
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) base station
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) latency
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) diff age
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) sol age
	uint8_t tempTracked = (uint8_t) atoi(strtok_r(NULL, ",", &saveIndex3));
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) num used in solution
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) GPS + GLONASS L1
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) GPS + GLONASS L1/L2
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) reserved for vendor
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) extended solution status
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) reserved for vendor
	token = strtok_r(NULL, ",", &saveIndex3); // (UNUSED) signals used

	//Removed the update time section of code

	if (!solSuccess) {
		Logger::Stream(LEVEL_DEBUG, tags) << "Invalid BESTXYZ, # of satellites: " << tempTracked;
		solSuccess = false;
		return;
	}

	Logger::Stream(LEVEL_INFO, tags) << "Good BESTXYZ data: " << tempData;


	//Convert into orbital elements for the propigator
	double r[3] = {0};
	double v[3] = {0};
	double rI[3] = {0};
	double vI[3] = {0};
	double gpsTime[2] = {0};
	r[0] = tempData.posX;
	r[1] = tempData.posY;
	r[2] = tempData.posZ;
	v[0] = tempData.velX;
	v[1] = tempData.velY;
	v[2] = tempData.velZ;
	gpsTime[0] = tempData.GPSWeek;
	gpsTime[1] = tempData.GPSSec;
	wgs2gcrf(r,v,gpsTime,rI,vI);
	float tempR[4] = {0};
	float tempV[4] = {0};
	for(int i = 0; i<3 ;i++){
		tempR[i+1] = (float)rI[i];
		tempV[i+1] = (float)vI[i];
	}
	lastLock.sysTime = getCurrentTime() /1000; //Store current time to use for prop
	rv2elem(MU_EARTH, tempR, tempV, &(lastLock.elements));
	Logger::Stream(LEVEL_DEBUG,tags) << "ELEMENTS: a: " << lastLock.elements.a << " e: " << lastLock.elements.e << " i: " << lastLock.elements.i << " O: " << lastLock.elements.Omega << " o: " << lastLock.elements.omega << " anom: " << lastLock.elements.anom;
	lastLock.GPSWeek = tempData.GPSWeek;
	lastLock.GPSSec = tempData.GPSSec;
	Logger::Stream(LEVEL_DEBUG,tags) << tempR[0]<< ','<< tempR[1] << ','<< tempR[2] << ','<< tempR[3];
	Logger::Stream(LEVEL_DEBUG,tags) << tempV[0]<< ','<<tempV[1]<<','<< tempV[2]<<','<< tempV[3];
	if(lockTries == 15){
		Logger::Stream(LEVEL_INFO,tags) << "Lock Found";
		isLocked = true;
		lockTries = 0;
		return;
	}
	lockTries++;
}

// TODO: Double check these two
uint32_t GPS::CRCValue_GPS(int i) {
	int j;
	uint32_t ulCRC;

	ulCRC = i;
	for (j = 8 ; j > 0; j--) {
		if (ulCRC & 1) {
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		} else {
			ulCRC >>= 1;
		}
	}
	return ulCRC;
}

// modified slightly from page 32 of http://www.novatel.com/assets/Documents/Manuals/om-20000094.pdf
uint32_t GPS::CalculateCRC_GPS(char * buffer) {
	uint32_t temp1;
	uint32_t temp2;
	uint32_t CRC = 0;

	buffer++;
	while (*buffer != '\0') {
		temp1 = (CRC >> 8) & 0x00FFFFFFL;
		temp2 = CRCValue_GPS(((int) CRC ^ ((uint8_t) *buffer++)) & 0xff);
		CRC = temp1 ^ temp2;
	}
	return CRC;
}

//! Only works for increments of less than a week
void GPS::incrementGPSTime(int32_t& GPSWeek, float& GPSSec, float dt){
	//TODO check and make sure this is actually correct?
	GPSSec += dt;
	if(GPSSec > 604800){
		GPSSec -= 604800;
		GPSWeek++;
	}
}

//! gets the check on whether or not the GPS gave a successful lock
bool GPS::getSuccess(){
	return solSuccess;
}

//! used to power on
void GPS::powerOn(){
	pow.powerOn();
	power = true;
}

//! used to power off
void GPS::powerOff(){
	pow.powerOff();
	power = false;
}

//! returns status of power
bool GPS::isOn(){
	return power;
}

//! returns status of the gps lock
bool GPS::getLockStatus(){
	return isLocked;
}
