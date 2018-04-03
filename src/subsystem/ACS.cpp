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
	health.fileSize = FileManager::MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY ACS_PATH "/ACS";
	pointingValid = false;
	TimeSinceLock = -1;
	TargetMRP.assign(3,0);
	ActualMRP.assign(3,0);

}

ACS::~ACS(){}

//! Will set up the Gpio lines, acp devices, handle configs, and test alive
bool ACS::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing ACS";
	LockGuard l(lock);
	subPower.powerOn();

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
	handleConfig();
	return true;
}

/*!
 * Handles any mode transition needs as well as any needs for tasks to be done in a mode.
 * \param Mode that FSW is in
 */
void ACS::handleMode(FSWMode transition){
	LockGuard l(lock);
	bool success;
	switch (transition){
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

//! Sends the configs to ACS
void ACS::handleConfig(){
	if(FileManager::checkExistance(ACS_CONFIG)){
		std::vector<uint8_t> buff = FileManager::readFromFile(ACS_CONFIG);
		if (buff.size() == ACS_CONFIG_SIZE){
			Logger::Stream(LEVEL_INFO,tags) << "Sending ACS Config";
			ACPPacket acpReturn = sendOpcode(OP_CONFIGGAINS,buff);
			if (!isSuccess(OP_CONFIGGAINS,acpReturn)){
				Logger::Stream(LEVEL_ERROR,tags) << "ACS did not receive ACS Config";
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect ACS Config file Size";
		}
	}else{
		Logger::Stream(LEVEL_ERROR,tags) << "ACS Config file does not exist";
	}
}

//! Sends the updated config to ACS
void ACS::updateConfig(){
	if(FileManager::checkExistance(ACS_CONFIG_UP)){
		std::vector<uint8_t> buff = FileManager::readFromFile(ACS_CONFIG_UP);
		if (buff.size() == ACS_CONFIG_SIZE){
			Logger::Stream(LEVEL_INFO,tags) << "Sending ACS Config Update";
			ACPPacket acpReturn = sendOpcode(OP_CONFIGGAINS,buff);
			if (!isSuccess(OP_CONFIGGAINS,acpReturn)){
				Logger::Stream(LEVEL_ERROR,tags) << "ACS did not receive ACS Config Update";
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect ACS Config Update file Size";
		}
	}
	else{
		Logger::Stream(LEVEL_WARN,tags) << "There are no ACS config updates";
	}

}


//! Handles the capturing and storing of the health and status for a subsystem
void ACS::getHealthStatus(){
	LockGuard l(lock);
	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS,buff);

	updateTimeSinceLock(acpReturn.message);

	health.recordBytes(acpReturn.message);
}

//! Change the current pointing target to Nadir
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

//! Change the current pointing target to Ground
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

//! Change the current pointing target to Sun Soak
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

/*!
 * Sends GPS data to ACS
 * \param ECI coordinates to be pasted
 */
bool ACS::sendGPS(GPSPositionTime gps){
	LockGuard l(lock);
	Logger::Stream(LEVEL_DEBUG,tags) << gps;
	SerializeGPS serGPS(gps);
	std::vector<uint8_t> buffer = serGPS.serialize();
	ACPPacket retPacket = sendOpcode(OP_SENDGPS, buffer);
	if (!isSuccess(OP_SENDGPS,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode send GPS: Unable to send GPS data to ACS. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;
}

//TODO: Do we need this?
void ACS::configureGains(){}

//! resets ACS
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

/*!
 * Handles the sending of opCodes
 * \param opcode to be sent
 * \param buffer to be sent (if need be)
 */
ACPPacket ACS::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
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

//! Checks the success of an ACS Specific opcode being sent
bool ACS::isSuccess(ACSOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

//! Checks the success of a Subsystem opcode being sent
bool ACS::isSuccess(SubsystemOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

//! returns the time since lock
float ACS::getTimeSinceLock(){
	return this->TimeSinceLock;
}

//! returns the target MRP
std::vector<float> ACS::getTargetMRP(){
	return this->TargetMRP;
}

std::vector<float> ACS::getActualMRP(){
	return this->ActualMRP;
}

/*!
 * Updates the time since lock
 * \param vector buffer containing info for updating
 */
void ACS::updateTimeSinceLock(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(35);
	bs >> TimeSinceLock;
}

/*!
 * Updates the Target MRP
 * \param vector buffer containing info for updating
 */
void ACS::updateTargetMRP(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(6);
	bs >> TargetMRP[0] >> TargetMRP[1] >> TargetMRP[2];


}

/*!
 * Updates the Actual MRP
 * \param vector buffer containing info for updating
 */
void ACS::updateActualMRP(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(3);
	bs >> ActualMRP[0] >> ActualMRP[1] >> ActualMRP[2];
}


