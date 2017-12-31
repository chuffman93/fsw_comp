/*
 * EPS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_EPS_H_
#define INCLUDE_SUBSYSTEM_EPS_H_

#include "subsystem/SubsystemBase.h"
//#include "interfaces/SubPowerInterface.h"
#include "interfaces/ACPInterface.h"


class EPS: public SubsystemBase{
public:
	EPS();
	~EPS();

	//Will set up the Gpio lines and the acp devices
	void initialize(){};
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition){};
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus(){};
private:
	//Power cycle the entire satellite
	void commandReset();
};


#endif /* INCLUDE_SUBSYSTEM_EPS_H_ */
