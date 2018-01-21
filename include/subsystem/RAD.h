/*
 * RAD.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_RAD_H_
#define INCLUDE_SUBSYSTEM_RAD_H_

#include "subsystem/SubsystemBase.h"
#include "core/ScheduleManager.h"
#include "interfaces/SubPowerInterface.h"
#include "interfaces/ACPInterface.h"
#include "core/FileSystem.h"
#include "core/FileManager.h"
#include "core/Lock.h"
#include "interfaces/ExternalProcess.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include <stdio.h>

#define RAD_CHUNK_SIZE		102400

enum PLDOpcode {
	OP_PLD_CMD_MIN = 32,
	OP_STARTSCIENCE = OP_PLD_CMD_MIN,
	OP_DATACMD = 33,
	OP_PLD_CMD_MAX = 34,

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

	ACPPacket sendOpcode(uint8_t opcode);

	std::string currentHealthFile;
	size_t healthFileSize;
	uint16_t RADDataNum;
PRIVATE:
	//Various configurations for the data collection
	void configMotor();
	void configData();

	uint16_t updateDataNumber();
	uint16_t readDataNumber();

	//Command the beginning of data collection
	void commandCollectionBegin();
	void commandCollectionEnd();
	char dataFile[100];

	ExternalProcess tftp;
	ACPInterface& acp;
	SubPowerInterface& subPower;
	Lock lock;
	LogTags tags;

};


#endif /* INCLUDE_SUBSYSTEM_RAD_H_ */
