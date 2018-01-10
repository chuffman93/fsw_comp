
/*
 * EPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */


#include "subsystem/EPS.h"
#include "core/FileManager.h"

#include <stdint.h>
#include <unistd.h>
#include <iostream>



EPS::EPS(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "FileManager");
	health.sync = EPS_SYNC;
	health.fileSize = MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY EPS_PATH "/EPS";
}

EPS::~EPS(){}

//Will set up the Gpio lines and the acp devices
void EPS::initialize(){
	//TODO: error handling

	LockGuard l(lock);
	ACPPacket acpPacket(EPS_SYNC, OP_TESTALIVE);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTLED;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTCONFIG;
	acp.transaction(acpPacket,acpReturn);
}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void EPS::handleMode(FSWMode transition){
	switch (transition) {
	case Mode_Reset:
		commandReset();
		break;
	default:
		break;
		//TODO: error handling
	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void EPS::getHealthStatus(){

	LockGuard l(lock);
	ACPPacket acpPacket(health.sync, OP_HEALTHSTATUS);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	std::string healthFile;
	size_t messageSize = acpReturn.message.size();
	if ((health.fileSize+messageSize) < MAX_FILE_SIZE){
		healthFile = health.currentFile;
		health.fileSize += messageSize;
	}else{
		healthFile = FileManager::createFileName(health.basePath);
		health.fileSize = messageSize;
	}
	FileManager::writeToFile(healthFile,acpReturn.message);

	ByteStream bs(acpReturn.message);
	bs.seek(12) >> batteryCharge;

}

//Power cycle the entire satellite
void EPS::commandReset(){
	//TODO: error handling
	LockGuard l(lock);
	ACPPacket acpPacket(EPS_SYNC, OP_SUBSYSTEMRESET);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	//sleep(5);

	//subPower.reset();

}

uint16_t EPS::getBatteryCapacity(){
	LockGuard l(lock);
	return batteryCharge;
}
