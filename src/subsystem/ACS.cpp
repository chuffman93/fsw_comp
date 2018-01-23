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
	health.fileSize = 0;
	health.basePath = HEALTH_DIRECTORY RAD_PATH "/ACS";
}

ACS::~ACS(){}

//Will set up the Gpio lines and the acp devices
void ACS::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing ACS";

	std::vector<uint8_t> buff;
	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
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

	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS,buff);

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
	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_POINTNADIR,buff);
}

void ACS::pointCOM(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to Ground";
	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_POINTCOM,buff);

}

void ACS::pointSunSoak(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to Sun Soak";
	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_POINTSUN,buff);
}


//Update the GPS information on ACS
void ACS::sendGPS(){

	SerializeGPS serGPS;
	std::vector<uint8_t> buffer = serGPS.serialize();

	ACPPacket retPacket = sendOpcode(OP_SENDGPS, buffer);
}

//Configure the gains on ACS
void ACS::configureGains(){

}

void ACS::resetACS(){
	Logger::Stream(LEVEL_INFO,tags) << "Preparing ACS for Reset";
	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);

}


ACPPacket ACS::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	LockGuard l(lock);
	if (buffer.empty()){
		ACPPacket acpPacket(ACS_SYNC, opcode);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}else{
		ACPPacket acpPacket(ACS_SYNC, opcode, buffer);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}
}


