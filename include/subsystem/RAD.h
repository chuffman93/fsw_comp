/*
 * RAD.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_RAD_H_
#define INCLUDE_SUBSYSTEM_RAD_H_

#include "subsystem/SubsystemBase.h"
#include "interfaces/SubPowerInterface.h"
#include "interfaces/ACPInterface.h"
#include "core/FileSystem.h"
#include "core/FileManager.h"
#include "core/Lock.h"

#include <stdio.h>

enum PLDOpcode {
	OP_STARTSCIENCE = 32,
	OP_DATACMD = 33,

	OP_CLOCKCONFIG = 64,
	OP_MOTORCONFIG = 65,
	OP_DATACONFIG = 66,

	OP_BACKUPSTARTSCIENCE = 96,
};

class RAD: public SubsystemBase{
public:
	RAD(ACPInterface& acp, SubPowerInterface& subPower);
	~RAD();

	//Handle power on and initialization routine
	void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();
	std::string currentHealthFile;
	size_t healthFileSize;
private:
	//Various configurations for the data collection
	void configMotor();
	void configData();
	//Command the beginning of data collection
	void commandCollectionBegin();
	void commandCollectionEnd();
	//Handles the initialization of the TFTP thread
	void TFTPLaunch();

	ACPInterface& acp;
	SubPowerInterface& subPower;
	Lock lock;
	LogTags tags;
};


#endif /* INCLUDE_SUBSYSTEM_RAD_H_ */
