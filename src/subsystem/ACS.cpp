/*
* ACS.cpp
*
*  Created on: Dec 22, 2017
*      Author: alex
*/

#include "subsystem/ACS.h"

ACS::ACS(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "ACS");
	pointingValid = false;
}

ACS::~ACS(){}

//Will set up the Gpio lines and the acp devices
void ACS::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing ACS";
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
		sendGPS();
		break;
	case Mode_Payload:
		sendGPS();
		break;
	case Mode_Com:
		sendGPS();
		break;
	case Mode_Reset:
		resetACS();
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
		break;

	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void ACS::getHealthStatus(){

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

//Change the current pointing target
void ACS::pointNadir(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to NADIR";
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_POINTNADIR);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

void ACS::pointCOM(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to Ground";
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_POINTCOM);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

void ACS::pointSunSoak(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to Sun Soak";
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_POINTSUN);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}


//Update the GPS information on ACS
void ACS::sendGPS(){
	LockGuard l(lock);

	SerializeGPS serGPS;
	std::vector<uint8_t> buffer = serGPS.serialize();

	ACPPacket acpPacket(ACS_SYNC, OP_SENDGPS, buffer);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}

//Configure the gains on ACS
void ACS::configureGains(){

}

void ACS::resetACS(){
	Logger::Stream(LEVEL_INFO,tags) << "Preparing ACS for Reset";
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, OP_SUBSYSTEMRESET);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
}


ACPPacket ACS::sendOpcode(uint8_t opcode){
	LockGuard l(lock);
	ACPPacket acpPacket(ACS_SYNC, opcode);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);
	return acpReturn;
}


