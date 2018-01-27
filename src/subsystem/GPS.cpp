/*
 * GPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#include "subsystem/GPS.h"

extern "C" {
	#include "util/propagator/AllStarOrbitProp.h"
	#include "util/propagator/gpsFrameRotation.h"
	#include "util/propagator/OrbitalMotionAllStar.h"
}

#include <string.h>


#define CRC32_POLYNOMIAL	0xEDB88320L

GPS::GPS(NMEAInterface& nm, SubPowerInterface& pow):nm(nm), pow(pow){
	tags += LogTag("Name", "GPS");
}


GPS::~GPS(){}

void GPS::initialize(){
	pow.configDelay(0, 1000);
	pow.powerOff();
	pow.powerOn();
}

void GPS::handleMode(FSWMode transition){}

void GPS::getHealthStatus(){

}

GPSPositionTime GPS::getBestXYZ(){
	GPSPositionTime inertial = getBestXYZI();


	// arrays for the rotation
	double posECI[3];
	double velECI[3];
	double posECEF[3];
	double velECEF[3];
	double gpsTime[2];

	posECI[0] = inertial.posX;
	posECI[1] = inertial.posY;
	posECI[2] = inertial.posZ;
	velECI[0] = inertial.velX;
	velECI[1] = inertial.velY;
	velECI[2] = inertial.velZ;
	gpsTime[0] = inertial.GPSWeek;
	gpsTime[1] = inertial.GPSSec;

	gcrf2wgs(posECI, velECI, gpsTime, posECEF, velECEF);

	GPSPositionTime retval;
	retval.posX = posECEF[0];
	retval.posY = posECEF[1];
	retval.posZ = posECEF[2];
	retval.velX = velECEF[0];
	retval.velY = velECEF[1];
	retval.velZ = velECEF[2];
	retval.GPSWeek = inertial.GPSWeek;
	retval.GPSSec = inertial.GPSSec;
	return retval;
}

GPSPositionTime GPS::getBestXYZI(){
	LockGuard l(lock);
	float eciPos[3];
	float eciVel[3];
	int64_t currTime = getFSWMillis();
	float propTime = currTime/1000.0 - lastLock.sysTime;

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
	incrementGPSTime(pt.GPSWeek, pt.GPSSec, propTime);
	return pt;
}

/*!
 * Waits to get new GPS coordinates then parses them
 */
void GPS::fetchNewGPS(){
	std::string data = nm.getString();

	//Everything below this line is from old FSW
	GPSPositionTime tempData = {0,0,0,0,0,0,0,0};
	char * token;
	char * buffPtr = (char*)data.c_str();
	bool solSuccess = true;
	bool updateTime = false;

	bool containsDelimiter = false;
	while ((buffPtr - data.c_str() != 350) && (*buffPtr != '\0')) {
		if (*buffPtr++ == '*') {
			containsDelimiter = true;
			break;
		}
	}

	if (!containsDelimiter) {
		Logger::log(LEVEL_WARN, tags, "String doesn't contain '*' ");
		return;
	}

	// split into message and crc
	char * message = strtok((char*)data.c_str(), "*");
	char * crcStr = strtok(NULL, "*");
	uint32_t crc = strtoul(crcStr, &token, 16);

	// validate crc
	if (crc != CalculateCRC_GPS(message)) {
		Logger::log(LEVEL_WARN, tags, "CRC Failed");
		//return false; TODO: when working with the simulator, we need to ignore the CRC
	}

	// parse the message into header and log
	char * header = strtok(message, ";");
	char * log = strtok(NULL, ";");

	// parse the message header
	if (strcmp("BESTXYZA", strtok(header, ",")) != 0) {
		Logger::log(LEVEL_WARN, tags, "Wrong message, expected BESTXYZA");
		return;
	}

	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) sequence num
	token = strtok(NULL, ","); // (UNUSED) idle time
	if (strcmp("UNKNOWN", strtok(NULL, ",")) != 0) { // time status is ok
		updateTime = true;
	}
	tempData.GPSWeek = (uint16_t) strtoul(strtok(NULL, ","), NULL, 10);
	tempData.GPSSec = strtof(strtok(NULL, ","), NULL);
	token = strtok(NULL, ","); // (UNUSED) receiver status
	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) software build

	// parse the position log part of the message
	if (strcmp("SOL_COMPUTED", strtok(log, ",")) != 0) {
		Logger::log(LEVEL_DEBUG, tags, "No position solution computed!");
		solSuccess = false;
	} else {
		Logger::log(LEVEL_DEBUG, tags, "Valid position solution computed!");
		token = strtok(NULL, ","); // (UNUSED) position type
		tempData.posX = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.posY = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.posZ = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		token = strtok(NULL, ","); // (UNUSED) std dev x
		token = strtok(NULL, ","); // (UNUSED) std dev y
		token = strtok(NULL, ","); // (UNUSED) std dev z
	}

	// parse the velocity log part of the message
	if (strcmp("SOL_COMPUTED", strtok(NULL, ",")) != 0) {
		Logger::log(LEVEL_DEBUG, tags, "No velocity solution computed!");
		solSuccess = false;
	} else {
		Logger::log(LEVEL_DEBUG, tags, "Velocity solution computed!");
		token = strtok(NULL, ","); // (UNUSED) velocity type
		tempData.velX = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.velY = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.velZ = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		token = strtok(NULL, ","); // (UNUSED) std dev x
		token = strtok(NULL, ","); // (UNUSED) std dev y
		token = strtok(NULL, ","); // (UNUSED) std dev z
	}

	// parse the rest of the log
	token = strtok(NULL, ","); // (UNUSED) base station
	token = strtok(NULL, ","); // (UNUSED) latency
	token = strtok(NULL, ","); // (UNUSED) diff age
	token = strtok(NULL, ","); // (UNUSED) sol age
	uint8_t tempTracked = (uint8_t) atoi(strtok(NULL, ","));
	token = strtok(NULL, ","); // (UNUSED) num used in solution
	token = strtok(NULL, ","); // (UNUSED) GPS + GLONASS L1
	token = strtok(NULL, ","); // (UNUSED) GPS + GLONASS L1/L2
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) extended solution status
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) signals used

	//Removed the update time section of code

	if (!solSuccess) {
		Logger::Stream(LEVEL_DEBUG, tags) << "Invalid BESTXYZ, # of satellites: " << tempTracked;
		return;
	}

	Logger::log(LEVEL_DEBUG, tags, "Good BESTXYZ data");


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
	lock.lock();
	float tempR[4] = {0};
	float tempV[4] = {0};
	for(int i = 0; i<3 ;i++){
		tempR[i+1] = (float)rI[i];
		tempV[i+1] = (float)vI[i];
	}
	lastLock.sysTime = getFSWMillis() / 1000.0; //Store current time to use for prop
	rv2elem(MU_EARTH, tempR, tempV, &(lastLock.elements));
	lastLock.GPSWeek = tempData.GPSWeek;
	lastLock.GPSSec = tempData.GPSSec;
	lock.unlock();
}

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

	while (*buffer != '\0') {
		temp1 = (CRC >> 8) & 0x00FFFFFFL;
		temp2 = CRCValue_GPS(((int) CRC ^ ((uint8_t) *buffer++)) & 0xff);
		CRC = temp1 ^ temp2;
	}

	return CRC;
}

uint32_t GPS::getFSWMillis(){
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t.tv_sec*1000 + t.tv_nsec/(1000*1000);
}

//! Only works for increments of less than a week
void GPS::incrementGPSTime(uint16_t& GPSWeek, float& GPSSec, float dt){
	//TODO check and make sure this is actually correct?
	GPSSec += dt;
	if(GPSSec > 604800){
		GPSSec -= 604800;
		GPSWeek++;
	}
}


