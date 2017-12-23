/*
 * COM.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_COM_H_
#define INCLUDE_SUBSYSTEM_COM_H_

#include "SubsystemBase.h"

class COM: public SubsystemBase{
public:
	COM(){}
	~COM(){}

	//ID for subsystem
	static SubsystemID id = SUBSYSTEM_ID_COM;
	//Will set up the Gpio lines and the acp devices
	virtual void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	virtual void handleMode(mode_t transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus();
private:
	//Configure the lithium radio
	void configureLithium();
	//Send the beacon
	void sendBeacon();
	//Need to figure out how the GND Communication stuff will work

};


#endif /* INCLUDE_SUBSYSTEM_COM_H_ */
