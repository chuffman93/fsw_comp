/*
 * SubsystemBase.h
 *
 *  Created on: Dec 22, 2017
 *      Author: fsw
 */

#ifndef SUBSYSTEMBASE_H_
#define SUBSYSTEMBASE_H_

#include <stdint.h>
#include "../hal/GPIOManager.h"

enum SubsystemID {
	SUBSYSTEM_ID_ACS,
	SUBSYSTEM_ID_CDH,
	SUBSYSTEM_ID_COM,
	SUBSYSTEM_ID_EPS,
	SUBSYSTEM_ID_GPS,
	SUBSYSTEM_ID_RAD
};

//typedef GPIODevice * gpioid;

class SubsystemBase{
public:
	SubsystemBase(){}
	virtual ~SubsystemBase(){}
	
	//gpioid reset;
	//gpioid power;

	//ID for subsystem
	static SubsystemID id;
	//Will set up the Gpio lines and the acp devices
	virtual void initialize() = 0;
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode. 
	virtual void handleMode(mode_t transition) = 0;
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus() = 0;
private:
	//Toggling gpios
	void powerOff();
	void powerOn();
	//Send no op code to ensure that subsystem is running
	bool testAlive();
	//Set up the debug LEDs
	bool commandLED(bool on, uint16_t rate);
	//Readback the commanded LED data
	bool dataLED(bool* on, uint16_t *rate);    
};

#endif /* SUBSYSTEMBASE_H_ */
