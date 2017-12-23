/*
 * GPS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_GPS_H_
#define INCLUDE_SUBSYSTEM_GPS_H_

#include "SubsystemBase.h"

struct GPSData {

};

class GPS: public SubsystemBase{
public:
	GPS(){}
	~GPS(){}

	Lock lock;
	//Uartdevid uart;
	//ID for subsystem
	static SubsystemID id = SUBSYSTEM_ID_GPS;
	//Handle power on and initialization routine
	virtual void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	virtual void handleMode(mode_t transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus();
private:
	//Get the current state of the GPS, with propagation. Take lock when reading from the info struct
	void getBestXYZ()
	void getLatLong()
	//Wait until we get a new string from the UART interface then write it into the GPSDataStruct. Timeout?
	void parseString();


};



#endif /* INCLUDE_SUBSYSTEM_GPS_H_ */
