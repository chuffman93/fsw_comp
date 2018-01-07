/*
 * EPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */


#include "subsystem/EPS.h"
#include "core/FileManager.h"
#include "core/FileInterfaces.h"

#include <stdint.h>
#include <unistd.h>



EPS::EPS(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower)
{ tags += LogTag("Name", "FileManager"); }

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
	//TODO: error handling
	LockGuard l(lock);
	ACPPacket acpPacket(EPS_SYNC, OP_HEALTHSTATUS);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	std::string folderLocation = HEALTH_DIRECTORY EPS_PATH;
	HealthStatusInterface health(folderLocation);
	health.pushData(acpReturn.message);

	std::vector<uint8_t> message = acpReturn.message;
	batteryCapacity = ((uint16_t)message[12] << 8) | ((uint16_t)message[13]);

}

//Power cycle the entire satellite
void EPS::commandReset(){
	//TODO: error handling
	LockGuard l(lock);
	ACPPacket acpPacket(EPS_SYNC, OP_SUBSYSTEMRESET);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	sleep(40);

	//ModeManager modeman;
	//modeman.switchModes(Mode_Bus);
}

uint16_t EPS::getBatteryCapacity(){
	LockGuard l(lock);
	return batteryCapacity;
}



