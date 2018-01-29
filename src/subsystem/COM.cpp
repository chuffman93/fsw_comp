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
	health.sync = COM_SYNC;
	health.fileSize = MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY COM_PATH "/COM";
	healthFileSize = 0;
}

COM::~COM(){}

//Will set up the Gpio lines and the acp devices
void COM::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing COM";

	ACPPacket acpPacket(COM_SYNC, OP_TESTALIVE);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTLED;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTCONFIG;
	acp.transaction(acpPacket,acpReturn);
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

	LockGuard l(lock);
	ACPPacket acpPacket(health.sync, OP_HEALTHSTATUS);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);


	health.recordBytes(acpReturn.message);
}

//Configure the lithium radio
void COM::configureLithium(){
		//set baud rate command
}

//Send the beacon
void COM::sendBeacon(){

}

void COM::resetCOM(){
	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Preparing COM for Reset";

	ACPPacket acpPacket(COM_SYNC, OP_SUBSYSTEMRESET);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

void COM::changeBaudRate(uint32_t baudRate){
	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Changing COM Baud Rate to " << baudRate;

	ByteStream bs;
	bs << baudRate;

	ACPPacket acpPacket(COM_SYNC, OP_HEALTHSTATUS, bs.vec());
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

//Need to figure out how the GND Communication stuff will work

ACPPacket COM::sendOpcode(uint8_t opcode){
	LockGuard l(lock);
	ACPPacket acpPacket(COM_SYNC, opcode);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
	return acpReturn;
}
