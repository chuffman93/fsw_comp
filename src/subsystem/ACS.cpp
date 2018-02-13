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
	health.fileSize = MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY ACS_PATH "/ACS";
	pointingValid = false;
	TimeSinceLock = -1;
	TargetMRP.assign(3,0);
	ActualMRP.assign(3,0);

}

ACS::~ACS(){}

//Will set up the Gpio lines and the acp devices
bool ACS::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing ACS";
	LockGuard l(lock);


	std::vector<uint8_t> buff;
	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
	if (!isSuccess(OP_TESTALIVE,retPacket1)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: ACS is not alive. Opcode Received: " << retPacket1.opcode;
		return false;
	}

	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
	if (!isSuccess(OP_TESTLED,retPacket2)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test LED: ACS is not alive. Opcode Received: " << retPacket2.opcode;
		return false;
	}

	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
	if (!isSuccess(OP_TESTCONFIG,retPacket3)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Configurations: ACS is not alive. Opcode Received: " << retPacket3.opcode;
		return false;
	}

	return true;
}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void ACS::handleMode(FSWMode transition){
	bool success;
	switch (transition){
	case Mode_Bus:
		success = sendGPS();
		break;
	case Mode_Payload:
		success = sendGPS();
		break;
	case Mode_Com:
		success = sendGPS();
		break;
	case Mode_Reset:
		success = resetACS();
		break;
	case Trans_BusToPayload:
		success = pointNadir();
		break;
	case Trans_PayloadToBus:
		success = pointSunSoak();
		break;
	case Trans_BusToCom:
		success =  pointCOM();
		break;
	case Trans_ComToBus:
		success =  pointSunSoak();
		break;
	default:
		break;
	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void ACS::getHealthStatus(){
	LockGuard l(lock);
	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS,buff);

	updateTimeSinceLock(acpReturn.message);

	health.recordBytes(acpReturn.message);
}

//Change the current pointing target
bool ACS::pointNadir(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to NADIR";
	std::vector<uint8_t> buff;

	ACPPacket retPacket = sendOpcode(OP_POINTNADIR,buff);
	if (!isSuccess(OP_POINTNADIR,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Point NADIR: Unable to point NADIR. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;
}

bool ACS::pointCOM(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to Ground";
	std::vector<uint8_t> buff;

	ACPPacket retPacket = sendOpcode(OP_POINTCOM,buff);
	if (!isSuccess(OP_POINTCOM,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Point COM: Unable to point COM. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;

}

bool ACS::pointSunSoak(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Command ACS to point to Sun Soak";
	std::vector<uint8_t> buff;

	ACPPacket retPacket = sendOpcode(OP_POINTSUN,buff);
	if (!isSuccess(OP_POINTSUN,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Point SUN: Unable to point SUN. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;
}


//Update the GPS information on ACS
bool ACS::sendGPS(){

	SerializeGPS serGPS;
	std::vector<uint8_t> buffer = serGPS.serialize();

	ACPPacket retPacket = sendOpcode(OP_SENDGPS, buffer);
	if (!isSuccess(OP_SENDGPS,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode send GPS: Unable to send GPS data to ACS. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;
}

//Configure the gains on ACS
void ACS::configureGains(){

}

bool ACS::resetACS(){
	Logger::Stream(LEVEL_INFO,tags) << "Preparing ACS for Reset";
	std::vector<uint8_t> buff;

	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
	if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: ACS not ready for reset. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;

}


ACPPacket ACS::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	//LockGuard l(lock);
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

bool ACS::isSuccess(ACSOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

bool ACS::isSuccess(SubsystemOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

float ACS::getTimeSinceLock(){
	return this->TimeSinceLock;
}

std::vector<float> ACS::getTargetMRP(){
	return this->TargetMRP;
}

std::vector<float> ACS::getActualMRP(){
	return this->ActualMRP;
}

void ACS::updateTimeSinceLock(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(35);
	bs >> TimeSinceLock;
}

void ACS::updateTargetMRP(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(6);
	bs >> TargetMRP[0] >> TargetMRP[1] >> TargetMRP[2];


}
void ACS::updateActualMRP(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(3);
	bs >> ActualMRP[0] >> ActualMRP[1] >> ActualMRP[2];
}


