/*
 * BeaconManager.cpp
 *
 *  Created on: Jan 14, 2018
 *      Author: chuffman93
 */

#include "util/BeaconManager.h"
#include "core/FileManager.h"

BeaconManager::BeaconManager(){}

BeaconManager::~BeaconManager(){}

void BeaconManager::createBeaconGPS(int32_t satTime, uint32_t epochNumber,int32_t GPSWeek,	float GPSSec, double xPosition,
			double yPosition, double zPosition, double xVelocity, double yVelocity, double zVelocity){
	bGPS.satTime = satTime;
	bGPS.epochNumber = epochNumber;
	bGPS.GPSWeek = GPSWeek;
	bGPS.GPSSec = GPSSec;
	bGPS.xPosition = xPosition;
	bGPS.yPosition = yPosition;
	bGPS.zPosition = zPosition;
	bGPS.xVelocity = xVelocity;
	bGPS.yVelocity = yVelocity;
	bGPS.zVelocity = zVelocity;
}

void BeaconManager::createBeaconSYS( int32_t satTime, uint32_t epochNumber, int32_t GPSWeek, float GPSSec, uint8_t systemMode,
			uint16_t radNumber, uint16_t batteryCap, int8_t acsMode, float memory, float cpu15){
	bSYS.satTime = satTime;
	bSYS.epochNumber = epochNumber;
	bSYS.GPSWeek = GPSWeek;
	bSYS.GPSSec = GPSSec;
	bSYS.systemMode = systemMode;
	bSYS.radNumber = radNumber;
	bSYS.batteryCap = batteryCap;
	bSYS.acsMode = acsMode;
	bSYS.memory = memory;
	bSYS.cpu15 = cpu15;
}

void BeaconManager::serializeBeacon(){
	ByteStream bsgps;
	ByteStream bssys;

	bsgps << bGPS.satTime << bGPS.epochNumber << bGPS.GPSWeek << bGPS.GPSSec << bGPS.xPosition << bGPS.yPosition << bGPS.zPosition << bGPS.xVelocity << bGPS.yVelocity << bGPS.zVelocity;
	bssys << bSYS.satTime << bSYS.epochNumber << bSYS.GPSWeek << bSYS.GPSSec << bSYS.systemMode << bSYS.radNumber << bSYS.batteryCap << bSYS.acsMode << bSYS.memory << bSYS.cpu15;

	this->GPS = bsgps.vec();
	this->SYS = bssys.vec();


}

void BeaconManager::sendBeaconSYS(){
	FileManager::writeToFile(BCN_SYS,SYS);
}

void BeaconManager::sendBeaconGPS(){
	FileManager::writeToFile(BCN_GPS,GPS);
}



