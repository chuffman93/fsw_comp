/*
 * GPS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_GPS_H_
#define INCLUDE_SUBSYSTEM_GPS_H_

#include "SubsystemBase.h"
#include "hal/UARTManager.h"
#include "interfaces/SubPowerInterface.h"
#include "interfaces/NMEAInterface.h"
#include "util/Logger.h"

struct GPSPositionTime{
  double posX;
  double posY;
  double posZ;
  double velX;
  double velY;
  double velZ;
  uint16_t GPSWeek;
  float GPSSec;
};

struct GPSLockType {
	classicElements elements;
	float sysTime;
	uint16_t GPSWeek;
	float GPSSec;
};

class GPS: public SubsystemBase{
public:
	GPS(NMEAInterface& nm, SubPowerInterface& pow);
	~GPS();

	//Handle power on and initialization routine
	void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();

	void fetchNewGPS();
	GPSPositionTime getBestXYZ();
	GPSPositionTime getBestXYZI();
	void getLatLong();

private:
	uint32_t CalculateCRC_GPS(char * buffer);
	uint32_t CRCValue_GPS(int i);
	void incrementGPSTime(uint16_t& GPSWeek, float& GPSSec, float dt);
	uint32_t getFSWMillis();

	//! The UART connected to the physical GPS
	NMEAInterface& nm;
	//! Power Interface for the GPS
	SubPowerInterface& pow;
	//! Lock for accessing anything in the GPSData structs
	Lock lock;
	//! Tags to use for logging
	LogTags tags;
	GPSLockType lastLock;
};



#endif /* INCLUDE_SUBSYSTEM_GPS_H_ */
