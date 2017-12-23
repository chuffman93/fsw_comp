/*
 * EPS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_EPS_H_
#define INCLUDE_SUBSYSTEM_EPS_H_

#include "SubsystemBase.h"

class EPS: public SubsystemBase{
public:
	EPS(){}
	~EPS(){}

	//ID for subsystem
	static SubsystemID id = SUBSYSTEM_ID_EPS;
	//Will set up the Gpio lines and the acp devices
	virtual void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	virtual void handleMode(mode_t transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus();
private:
	//Power cycle the entire satellite
	void commandReset();
};


#endif /* INCLUDE_SUBSYSTEM_EPS_H_ */
