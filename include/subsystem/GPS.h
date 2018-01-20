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

struct GPSInertial{
  double posX;
  double posY;
  double posZ;
  double velX;
  double velY;
  double velZ;
  uint16_t GPSWeek;
  float GPSSec;
  uint8_t isAccurate;
};

class GPS: public SubsystemBase {
public:
	GPS(UARTManager& uart):uart(uart){}
	~GPS(){}

	//Handle power on and initialization routine
	MOCK void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	MOCK void getHealthStatus();
private:
	//Get the current state of the GPS, with propagation. Take lock when reading from the info struct
	void getBestXYZ();
	void getLatLong();
	//Wait until we get a new string from the UART interface then write it into the GPSDataStruct. Timeout?
	void parseString();

	//! The UART connected to the physical GPS
	UARTManager& uart;
	//! Lock for accessing anything in the GPSData structs
	Lock lock;
	GPSPositionTime postime;
	GPSInertial postimei;
};



#endif /* INCLUDE_SUBSYSTEM_GPS_H_ */
