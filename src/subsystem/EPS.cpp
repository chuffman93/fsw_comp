
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
	tags += LogTag("Name", "EPS");
	health.fileSize = 0;
	health.basePath = HEALTH_DIRECTORY EPS_PATH "/EPS";
	batteryCharge = 0;
}

EPS::~EPS(){}

//Will set up the Gpio lines and the acp devices
void EPS::initialize(){
	//TODO: error handling

	Logger::Stream(LEVEL_INFO,tags) << "Initializing EPS";

	std::vector<uint8_t> buff;
	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
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

	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS, buff);

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

ACPPacket EPS::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	LockGuard l(lock);
	if (buffer.empty()){
		ACPPacket acpPacket(EPS_SYNC, opcode);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}else {
		ACPPacket acpPacket(EPS_SYNC, opcode, buffer);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}
}


//Power cycle the entire satellite
void EPS::commandReset(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Reseting EPS";
	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS, buff);
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);

	//sleep(5);

	//subPower.reset();

}

uint16_t EPS::getBatteryCapacity(){
	return batteryCharge;
}
