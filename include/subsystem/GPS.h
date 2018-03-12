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

extern "C" {
	#include "util/propagator/AllStarOrbitProp.h"
	#include "util/propagator/gpsFrameRotation.h"
	#include "util/propagator/OrbitalMotionAllStar.h"
}

#define CONFIG_GPS_SIZE 4

struct GPSPositionTime{
  double posX;
  double posY;
  double posZ;
  double velX;
  double velY;
  double velZ;
  int32_t GPSWeek;
  float GPSSec;
  bool isAccurate;

  friend std::ostream &operator<<( std::ostream &output, const GPSPositionTime &g);
};

struct GPSLockType {
	classicElements elements;
	float sysTime;
	int32_t GPSWeek;
	float GPSSec;
};

class GPS: public SubsystemBase {
public:
	GPS(NMEAInterface& nm, SubPowerInterface& pow);
	~GPS();

	//Handle power on and initialization routine
	MOCK bool initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();
	void handleConfig();
	void updateConfig();
	bool getSuccess();
	void fetchNewGPS();
	GPSPositionTime getBestXYZI();
	void powerOn();
	void powerOff();
	bool getLockStatus();
	bool isOn();
	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	uint16_t timeout;
	uint16_t timein;


private:
	uint32_t CalculateCRC_GPS(char * buffer);
	uint32_t CRCValue_GPS(int i);
	void incrementGPSTime(int32_t& GPSWeek, float& GPSSec, float dt);
	void setConfigs();
	bool solSuccess;
	bool power;
	bool isLocked;
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
