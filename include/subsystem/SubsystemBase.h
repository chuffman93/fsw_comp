/*
 * SubsystemBase.h
 *
 *  Created on: Dec 22, 2017
 *      Author: fsw
 */

#ifndef SUBSYSTEMBASE_H_
#define SUBSYSTEMBASE_H_

#include "core/ScheduleManager.h"
#include "util/ByteStream.h"
#include "core/FileManager.h"
#include "interfaces/ACPInterface.h"


struct HealthFileStruct {
	std::string currentFile;
	std::string basePath;
	size_t fileSize;

	void recordBytes(std::vector<uint8_t> message);
};

class SubsystemBase{
public:
	SubsystemBase(){};
	virtual ~SubsystemBase(){};
	
	//Will set up the Gpio lines and the acp devices
	virtual void initialize() = 0;
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode. 
	virtual void handleMode(FSWMode transition) = 0;
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus() = 0;
	//Handles the config retrieval and sending
	virtual void handleConfig() = 0;
	virtual void updateConfig() = 0;

	virtual ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer) = 0;

	HealthFileStruct health;
private:
	Lock lock;
};

#endif /* SUBSYSTEMBASE_H_ */
