/*
 * RAD.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_RAD_H_
#define INCLUDE_SUBSYSTEM_RAD_H_

#include "SubsystemBase.h"

class RAD: public SubsystemBase{
public:
	RAD(){};
	~RAD(){};

	//ID for subsystem
	SubsystemID id  = SUBSYSTEM_ID_RAD;
	//Handle power on and initialization routine
	void initialize(){};
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition){};
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus(){};
private:
	//Various configurations for the data collection
	void configMotor();
	void configData();
	//Command the beginning of data collection
	void commandCollectionBegin();
	void commandCollectionEnd();
	//Handles the initialization of the TFTP thread
	void TFTPLaunch();


};


#endif /* INCLUDE_SUBSYSTEM_RAD_H_ */
