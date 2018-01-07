/*
* ACS.cpp
*
*  Created on: Dec 22, 2017
*      Author: alex
*/

#include "subsystem/ACS.h"

ACS::ACS(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower)
{ tags += LogTag("Name", "FileManager"); }

ACS::~ACS(){}

//Will set up the Gpio lines and the acp devices
void ACS::initialize(){
	//TODO: error handling

	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_TESTALIVE);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTLED;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTCONFIG;
	acp.transaction(acpPacket,acpReturn);
}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void ACS::handleMode(FSWMode transition){
	switch (transition){
	case Mode_Bus:
		configGPS();
		break;
	case Mode_Payload:
		configGPS();
		break;
	case Mode_Com:
		configGPS();
		break;
	case Mode_Reset:
		//TODO: reset
		break;
	case Trans_BusToPayload:
		pointNadir();
		break;
	case Trans_PayloadToBus:
		pointSunSoak();
		break;
	case Trans_BusToCom:
		pointCOM();
		break;
	case Trans_ComToBus:
		pointSunSoak();
		break;
	default:
		//TODO: error handling
		break;

	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void ACS::getHealthStatus(){
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_HEALTHSTATUS);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	std::string folderLocation = HEALTH_DIRECTORY ACS_PATH;
	HealthStatusInterface health(folderLocation);
	health.pushData(acpReturn.message);

}

//Change the current pointing target
void ACS::pointNadir(){
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_POINTNADIR);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

void ACS::pointCOM(){
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_POINTCOM);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

void ACS::pointSunSoak(){
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_POINTSUN);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}


//Update the GPS information on ACS
void ACS::configGPS(){

}

//Configure the gains on ACS
void ACS::configureGains(){

}
