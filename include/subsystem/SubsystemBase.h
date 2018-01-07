/*
 * SubsystemBase.h
 *
 *  Created on: Dec 22, 2017
 *      Author: fsw
 */

#ifndef SUBSYSTEMBASE_H_
#define SUBSYSTEMBASE_H_

#include "core/ScheduleManager.h"
#include "core/FileInterfaces.h"


class SubsystemBase{
public:
	virtual ~SubsystemBase(){};
	
	//Will set up the Gpio lines and the acp devices
	virtual void initialize() = 0;
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode. 
	virtual void handleMode(FSWMode transition) = 0;
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus() = 0;
private:
};

#endif /* SUBSYSTEMBASE_H_ */
