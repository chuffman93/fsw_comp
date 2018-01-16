/*
 * BeaconManager.h
 *
 *  Created on: Jan 10, 2018
 *      Author: chuffman93
 */

#ifndef INCLUDE_UTIL_BEACONMANAGER_H_
#define INCLUDE_UTIL_BEACONMANAGER_H_

#include "test/mockhal/MockSPIManager.h"
#include "subsystem/ACS.h"
#include "subsystem/EPS.h"
#include "subsystem/COM.h"
#include "util/TimeKeeper.h"
#include "util/ByteStream.h"
#include <vector>
#include <stdint.h>
#include <sys/sysinfo.h>



struct beaconGPS{
	int32_t satTime;
	uint32_t epochNumber;
	int32_t GPSWeek;
	float GPSSec;
	double xPosition;
	double yPosition;
	double zPosition;
	double xVelocity;
	double yVelocity;
	double zVelocity;
};

struct beaconSYS{
	int32_t satTime;
	uint32_t epochNumber;
	int32_t GPSWeek;
	float GPSSec;
	uint8_t systemMode;
	uint16_t radNumber;
	uint16_t batteryCap;
	int8_t acsMode;
	float memory;
	float cpu15;
};

class BeaconManager {
public:
	BeaconManager();
	~BeaconManager();
	void createBeaconGPS( int32_t satTime, uint32_t epochNumber,int32_t GPSWeek,	float GPSSec, double xPosition,
			double yPosition, double zPosition, double xVelocity, double yVelocity, double zVelocity);
	void createBeaconSYS( int32_t satTime, uint32_t epochNumber, int32_t GPSWeek, float GPSSec, uint8_t systemMode,
			uint16_t radNumber, uint16_t batteryCap, int8_t acsMode, float memory, float cpu15);
	void serializeBeacon();


	/*
	 * TODO: Not Sure these two are needed...
	 */
	// void sendBeaconSYS();
	// void sendBeaconGPS();




PRIVATE:
	beaconGPS bGPS;
	beaconSYS bSYS;
	std::vector<uint8_t> GPS;
	std::vector<uint8_t> SYS;
	int32_t beaconRate;
	int32_t increasedBeaconRate;



};


#endif /* INCLUDE_UTIL_BEACONMANAGER_H_ */
