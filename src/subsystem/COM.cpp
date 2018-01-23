/*
 * COM.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */
#include "subsystem/COM.h"


COM::COM(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "CDH");
	health.fileSize = 0;
	health.basePath = HEALTH_DIRECTORY COM_PATH "/COM";
}

COM::~COM(){}

//Will set up the Gpio lines and the acp devices
void COM::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing COM";

	std::vector<uint8_t> buff;
	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void COM::handleMode(FSWMode transition){
	switch (transition){
	case Mode_Reset:
		resetCOM();
		break;
	default:
		//TODO: error handling
		break;
	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void COM::getHealthStatus(){

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

}

//Configure the lithium radio
void COM::configureLithium(){
		//set baud rate command
}

//Send the beacon
void COM::sendBeacon(){

}

void COM::resetCOM(){

	Logger::Stream(LEVEL_INFO,tags) << "Preparing COM for Reset";

	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
}

void COM::changeBaudRate(uint32_t baudRate){

	Logger::Stream(LEVEL_INFO,tags) << "Changing COM Baud Rate to " << baudRate;

	ByteStream bs;
	bs << baudRate;

	ACPPacket retPacket = sendOpcode(OP_HEALTHSTATUS, bs.vec());

}

//Need to figure out how the GND Communication stuff will work

ACPPacket COM::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	LockGuard l(lock);
	if (buffer.empty()){
		ACPPacket acpPacket(COM_SYNC, opcode);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}else{
		ACPPacket acpPacket(COM_SYNC, opcode, buffer);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}
}
