/*
 * RAD.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "subsystem/RAD.h"

RAD::RAD(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "RAD");
	health.sync = RAD_SYNC;
	health.fileSize = MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY RAD_PATH "/RAD";
}

RAD::~RAD(){}

//Handle power on and initialization routine
void RAD::initialize(){
	//TODO: error handling

	//read data number

	//boot config

}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void RAD::handleMode(FSWMode transition){
	switch (transition){
	case Mode_Reset:
		//TODO: reset
		break;
	case Trans_BusToPayload:
		commandCollectionBegin();
		break;
	case Trans_PayloadToBus:
		commandCollectionEnd();
		break;
	default:
		break;
	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void RAD::getHealthStatus(){

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
}


//Various configurations for the data collection
void RAD::configMotor(){

}

void RAD::configData(){

}

//Command the beginning of data collection
void RAD::commandCollectionBegin(){
	//TODO: error handling

	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Beginning RAD Science Collection";
	ACPPacket acpPacket(RAD_SYNC, OP_TESTALIVE);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTLED;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTCONFIG;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_STARTSCIENCE;
	acp.transaction(acpPacket, acpReturn);
}


void RAD::commandCollectionEnd(){
	//TODO: error handling

	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Ending RAD Science Collection";
	ACPPacket acpPacket(RAD_SYNC, OP_SUBSYSTEMRESET);
	ACPPacket acpReturn;
	acp.transaction(acpPacket, acpReturn);

	subPower.powerOff();

	//Kill RAD TFTP process if still running, compress and move
	//the data, split that data

}

//Handles the initialization of the TFTP thread
void RAD::TFTPLaunch(){

}


