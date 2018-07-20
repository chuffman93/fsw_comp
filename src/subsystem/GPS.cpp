/*
 * GPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#include "subsystem/GPS.h"

#include <string.h>
#include "util/TimeKeeper.h"
#include "subsystem/EPS.h"
#include <stdio.h>

#define CRC32_POLYNOMIAL	0xEDB88320L

std::ostream &operator<<(std::ostream &output, const GPSPositionTime &g ) {

	char buf[200];
	sprintf(buf, "{x:%f, y:%f, z:%f, vx:%f, vy:%f, vz:%f, w:%d, s:%f}", g.posX, g.posY, g.posZ, g.velX, g.velY, g.velZ, g.GPSWeek, g.GPSSec);
    output << buf;
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
	pt.posX = 0;
	pt.posY = 0;
	pt.posZ = 0;
	pt.velX = 0;
	pt.velY = 0;
	pt.velZ = 0;
	pt.GPSWeek = 0;
	pt.GPSSec = 0;
	pt.isAccurate = 0;
	propTime = 0;
	beaconOut = false;
	bOut = 0;
	xlow = -3800;
	xhigh = 1400;
	ylow = -6600;
	yhigh = -3000;
	zlow = 1400;
	zhigh = 6000;
	stcl = false;
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
		bs >> timeout >> timein>>bOut;
		Logger::Stream(LEVEL_INFO,tags) << "Setting timeout to " << timeout/60 << " mins and timein to " << timein/3600 << " hrs";
		switch(bOut){
			case 0:
				Logger::Stream(LEVEL_INFO,tags) << "Beaconing worldwide.";
				beaconOut = true;
				break;
			case 1:
				Logger::Stream(LEVEL_INFO,tags) << "Only beaconing within range.";
				break;
			case 2:
				Logger::Stream(LEVEL_INFO,tags) << "Will not beacon.";
				if(FileManager::checkExistance(BEACON)){
					FileManager::deleteFile(BEACON);
				}
				break;
			default:
				Logger::Stream(LEVEL_INFO,tags) << "Beaconing worldwide.";
				beaconOut = true;
		}
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
		bs >> timeout >> timein>>bOut;
		Logger::Stream(LEVEL_INFO,tags) << "Updating timeout to " << timeout/60 << " mins and timein to " << timein/3600 << " hrs";
		switch(bOut){
			case 0:
				Logger::Stream(LEVEL_INFO,tags) << "Switching to beaconing worldwide.";
				beaconOut = true;
				break;
			case 1:
				Logger::Stream(LEVEL_INFO,tags) << "Switching to only beaconing within range.";
				break;
			case 2:
				Logger::Stream(LEVEL_INFO,tags) << "Switching to will not beacon.";
				if(FileManager::checkExistance(BEACON)){
					FileManager::deleteFile(BEACON);
				}
				break;
			default:
				Logger::Stream(LEVEL_INFO,tags) << "Switching to beaconing worldwide.";
				beaconOut = true;
		}
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
	float currTime = getCurrentTime();
	propTime = currTime - lastLock.sysTime;
	Logger::Stream(LEVEL_DEBUG,tags) << "PropTime: " << propTime << " currTime: " << currTime << " lastLock.sysTime: " << lastLock.sysTime;
	propagatePositionVelocity(lastLock.elements, propTime, eciPos, eciVel);
	pt.posX = eciPos[0];
	pt.posY = eciPos[1];
	pt.posZ = eciPos[2];
	pt.velX = eciVel[0];
	pt.velY = eciVel[1];
	pt.velZ = eciVel[2];
	pt.GPSSec = lastLock.GPSSec;
	pt.GPSWeek = lastLock.GPSWeek;
	pt.isAccurate = 1;
	// Used for finding whether or not we are in range of the ground station, by converting ECI back to ECEF
	Logger::Stream(LEVEL_DEBUG,tags) <<"ECI: "<< pt;
	Logger::Stream(LEVEL_DEBUG,tags) << lastLock.elements.a <<','<< lastLock.elements.e <<','<< lastLock.elements.i<< ','<< lastLock.elements.Omega<<',' << lastLock.elements.omega<< ',' << lastLock.elements.anom;
	incrementGPSTime(pt.GPSWeek, pt.GPSSec, propTime);
	if(bOut == 1){
		double gpsTime[2] = {0};
		gpsTime[0] = pt.GPSWeek;
		gpsTime[1] = pt.GPSSec;
		double rF[3] = {0};
		double vF[3] = {0};
		double eiPos[3];
		double eiVel[3];
		eiPos[0] = pt.posX;
		eiPos[1] = pt.posY;
		eiPos[2] = pt.posZ;
		eiVel[0] = pt.velX;
		eiVel[1] = pt.velY;
		eiVel[2] = pt.velZ;
		gcrf2wgs(eiPos,eiVel,gpsTime,rF,vF);
		//TODO: get values to put in.
		if((rF[0] > xlow  && rF[0] < xhigh) && (rF[1] > ylow && rF[1] < yhigh) && (rF[2] > zlow && rF[2] < zhigh )){
			Logger::Stream(LEVEL_DEBUG,tags) << "Within range of Ground Station";
			Logger::Stream(LEVEL_DEBUG,tags) << rF[0] << " " << rF[1] << " " << rF[2] << " " << vF[0] << " " << vF[1] << " " << vF[2];
			beaconOut = true;
		}else{
			if(FileManager::checkExistance(BEACON)){
				FileManager::deleteFile(BEACON);
			}
			Logger::Stream(LEVEL_DEBUG,tags) << "Not within range of Ground Station";
			beaconOut = false;
		}
	}
	return pt;
}

uint64_t GPS::calcSleepTime(GPSPositionTime st){
	float fkprop = getCurrentTime();
	float eciPos[3];
	float eciVel[3];
	double rF[3] = {0};
	double vF[3] = {0};
	double eiPos[3];
	double eiVel[3];
	double gpsTime[2] = {0};
	Logger::Stream(LEVEL_DEBUG,tags)<<" lastLock.sysTime: " << lastLock.sysTime;
	propagatePositionVelocity(lastLock.elements, fkprop, eciPos, eciVel);
	st.posX = eciPos[0];
	st.posY = eciPos[1];
	st.posZ = eciPos[2];
	st.velX = eciVel[0];
	st.velY = eciVel[1];
	st.velZ = eciVel[2];
	gpsTime[1] = st.GPSSec;
	gpsTime[0] = st.GPSWeek;
	st.GPSSec = lastLock.GPSSec;
	st.GPSWeek = lastLock.GPSWeek;
	eiPos[0] = st.posX;
	eiPos[1] = st.posY;
	eiPos[2] = st.posZ;
	eiVel[0] = st.velX;
	eiVel[1] = st.velY;
	eiVel[2] = st.velZ;
	gcrf2wgs(eiPos,eiVel,gpsTime,rF,vF);
	incrementGPSTime(st.GPSWeek, st.GPSSec, fkprop);
	while(!((rF[0] > xlow  && rF[0] < xhigh) && (rF[1] > ylow && rF[1] < yhigh) && (rF[2] > zlow && rF[2] < zhigh ))){
		if(fkprop > 86400){
			Logger::Stream(LEVEL_DEBUG,tags) << "Time Out for Calc Sleep exceeded";
			return -1;
		}
		propagatePositionVelocity(lastLock.elements, fkprop, eciPos, eciVel);
		st.posX = eciPos[0];
		st.posY = eciPos[1];
		st.posZ = eciPos[2];
		st.velX = eciVel[0];
		st.velY = eciVel[1];
		st.velZ = eciVel[2];
		gpsTime[1] = st.GPSSec;
		gpsTime[0] = st.GPSWeek;
		st.GPSSec = lastLock.GPSSec;
		st.GPSWeek = lastLock.GPSWeek;
		eiPos[0] = st.posX;
		eiPos[1] = st.posY;
		eiPos[2] = st.posZ;
		eiVel[0] = st.velX;
		eiVel[1] = st.velY;
		eiVel[2] = st.velZ;
		gcrf2wgs(eiPos,eiVel,gpsTime,rF,vF);
		incrementGPSTime(st.GPSWeek, st.GPSSec, fkprop);
		Logger::Stream(LEVEL_DEBUG,tags) << "prop: " << fkprop << "\nECEF x: "<< rF[0] << " y: " <<
				rF[1] << " z: " << rF[2] << " x: " << vF[0] << " y: " << vF[1] << " z: " << vF[2] << "\n" << st; // << "\nECI: " << st;
		fkprop++;
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "fkProp: " << fkprop << " getCurrentTime(): " << getCurrentTime();
	uint32_t sleepTime = fkprop - getCurrentTime();
	return sleepTime;
}

GPSPositionTime GPS::getPositionTime(){
	LockGuard l(lock);
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
		Logger::Stream(LEVEL_DEBUG,tags) << tempData;
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
		Logger::Stream(LEVEL_DEBUG,tags) << tempData;
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
	// used to check if in range to beacon
	if(bOut == 1){
		if((r[0] > xlow && r[0] < xhigh) && (r[1] > ylow && r[1] < yhigh) && (r[2] > zlow && r[2] < zhigh)){
			beaconOut = true;
		}else
			beaconOut = false;
	}
	lastLock.sysTime = getCurrentTime(); //Store current time to use for prop
	rv2elem(MU_EARTH, tempR, tempV, &(lastLock.elements));
	Logger::Stream(LEVEL_DEBUG,tags) << tempR[1] << ", " << tempR[2] << ", " << tempR[3];
	Logger::Stream(LEVEL_DEBUG,tags) << tempV[1] << ", " << tempV[2] << ", " << tempV[3];
	Logger::Stream(LEVEL_DEBUG,tags) << "ELEMENTS: a: " << lastLock.elements.a << " e: " << lastLock.elements.e << " i: " << lastLock.elements.i << " O: " << lastLock.elements.Omega << " o: " << lastLock.elements.omega << " anom: " << lastLock.elements.anom;
	lastLock.GPSWeek = tempData.GPSWeek;
	lastLock.GPSSec = tempData.GPSSec;
	propTime = 0;
	if(lockTries == 15){
		Logger::Stream(LEVEL_INFO,tags) << "Lock Found";
		isLocked = true;
		lockTries = 0;
		return;
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "Lock tries: " << lockTries;
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
	GPSSec = GPSSec + dt;
	if(GPSSec > 604800){
		GPSSec = GPSSec - 604800;
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

void GPS::resetLock(){
	isLocked = false;
}

//! returns status of power
bool GPS::isOn(){
	return power;
}

//! returns status of the gps lock
bool GPS::getLockStatus(){
	return isLocked;
}
