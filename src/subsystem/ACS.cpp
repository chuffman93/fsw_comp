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
	pointingValid = false; // this should be removed
	TimeSinceLock = -1;
	starMRP = 0;
	TargetMRP.assign(3,0);
	ActualMRP.assign(3,0);
	xPixel.assign(20,0);
	yPixel.assign(20,0);
	catalogID.assign(20,0);
	numStarsFound = 0;
	isOn = false;

}

ACS::~ACS(){}

/*
 * Powers ACS on, sends Test Alive, Test LED and Test Config opcodes to ACS, handles ACS configurations
 */
void ACS::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing ACS";
	//LockGuard l(lock);
	subPower.powerOn();
	isOn = true;

	std::vector<uint8_t> buff;
	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
	if (!isSuccess(OP_TESTALIVE,retPacket1)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: ACS is not alive. Opcode Received: " << retPacket1.opcode;
	}

	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
	if (!isSuccess(OP_TESTLED,retPacket2)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test LED: ACS is not alive. Opcode Received: " << retPacket2.opcode;
	}

	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
	if (!isSuccess(OP_TESTCONFIG,retPacket3)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Configurations: ACS is not alive. Opcode Received: " << retPacket3.opcode;
	}
	handleConfig();
}

/*!
 * Handles any mode transition needs as well as any needs for tasks to be done in a mode.
 * \param Mode that FSW is in
 * Most of the transitions should be taken out since they are irrelevant
 */
void ACS::handleMode(FSWMode transition){
	LockGuard l(lock);

	bool success;
	switch (transition){
	case Mode_Reset:
		success = resetACS(); //the return was going to be used for error handling, we should fix this since the returns are irrelevant
		break;
	case Trans_BusToADS:
		initialize();
		break;
	case Trans_BusToPayload:
		//success = pointNadir();
		break;
	case Trans_PayloadToBus:
		//success = pointSunSoak();
		break;
	case Trans_BusToCom:
		//success =  pointCOM();
		break;
	case Trans_ComToBus:
		//success =  pointSunSoak();
		break;
	case Trans_DetumbleToBus:
		//success = pointSunSoak();
		break;
	case Trans_ADSToBus:
		subPower.powerOff();
		FileManager::deleteFile(ADS_MODE);
		isOn = false;
		break;
	default:
		break;
	}
}

/*
 * Sends config to ACS
 */
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
			Logger::Stream(LEVEL_DEBUG,tags) << "ELSE....";
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


//! Handles the capturing and storing of the health and status for ACS
void ACS::getHealthStatus(){
	LockGuard l(lock);

	if (isACSOn()){
		std::vector<uint8_t> buff;
		ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS,buff);

		checkACSMode(acpReturn.message);
		updateStarMRP(acpReturn.message);
		updateTargetMRP(acpReturn.message);
		updateActualMRP(acpReturn.message);
		updateTimeSinceLock(acpReturn.message);
		updateXPixel(acpReturn.message);
		updateYPixel(acpReturn.message);
		updateCatalogID(acpReturn.message);
		updateNumStarsFound(acpReturn.message);

		health.recordBytes(acpReturn.message);
	}
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
	if (isACSOn()){
		Logger::Stream(LEVEL_DEBUG,tags) << gps;
		SerializeGPS serGPS(gps);
		std::vector<uint8_t> buffer = serGPS.serialize();
		ACPPacket retPacket = sendOpcode(OP_SENDGPS, buffer);
		if (!isSuccess(OP_SENDGPS,retPacket)){
			Logger::Stream(LEVEL_FATAL,tags) << "Opcode send GPS: Unable to send GPS data to ACS. Opcode Received: " << retPacket.opcode;
			return false;
		}
		return true;
	}else{
		return true;
	}
}

//TODO: Do we need this? .. no .... this should be removed
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
	LockGuard l(lock);
	return this->TimeSinceLock;
}

//! returns the target MRP
std::vector<float> ACS::getTargetMRP(){
	LockGuard l(lock);
	return this->TargetMRP;
}

/*
 * returns the actual MRP
 */
std::vector<float> ACS::getActualMRP(){
	LockGuard l(lock);
	return this->ActualMRP;
}

/*
 * returns the star MRP
 */
float ACS::getStarMRP(){
	LockGuard l(lock);
	return this->starMRP;
}

/*
 * returns X pixels
 */
std::vector<uint16_t> ACS::getXPixel(){
	LockGuard l(lock);
	return this->xPixel;
}

/*
 * returns Y pixels
 */
std::vector<uint16_t> ACS::getYPixel(){
	LockGuard l(lock);
	return this->yPixel;
}

/*
 * returns catalog ID
 */
std::vector<uint16_t> ACS::getCatalogID(){
	LockGuard l(lock);
	return this->catalogID;
}

/*
 * returns the number of start found
 */
uint8_t ACS::getNumStarsFound(){
	LockGuard l(lock);
	return this->numStarsFound;
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

/*
 * Saves current ACS mode from health and status for the beacon
 * \param ACS health and status buffer
 */
void ACS::checkACSMode(std::vector<uint8_t> buffer){
	uint8_t modeACS;
	ByteStream bs(buffer);
	bs.seek(1); //!!!!!!this or the star MRP is incorrect!!!!!!
	bs >> modeACS;

	Logger::Stream(LEVEL_DEBUG,tags) << "modeACS: " << (int)modeACS;
	if ((int)modeACS != 0) {
		sch->acsDetumble = false;
	}
}

/*
 * Saves current star MRP from health and status for the beacon
 * \param ACS health and status buffer
 */
void ACS::updateStarMRP(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(1); //!!!!!!!!!!!this or the ACS mode is incorrect!!!!!!!!!!!
	bs >> starMRP;
}

/*
 * Saves current x pixels from health and status for the beacon
 * \param ACS health and status buffer
 */
void ACS::updateXPixel(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(161);

	for (int i = 0; i<20; i++){
		bs >> xPixel[i];
	}
}

/*
 * Saves current y pixels from health and status for the beacon
 * \param ACS health and status buffer
 */
void ACS::updateYPixel(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(201);

	for (int i = 0; i<20; i++){
		bs >> yPixel[i];
	}
}

/*
 * Saves current catalog ID from health and status for the beacon
 * \param ACS health and status buffer
 */
void ACS::updateCatalogID(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(241);

	for (int i = 0; i<20; i++){
		bs >> catalogID[i];
	}
}

/*
 * Saves current number of stars found from health and status for the beacon
 * \param ACS health and status buffer
 */
void ACS::updateNumStarsFound(std::vector<uint8_t> buffer){
	ByteStream bs(buffer);
	bs.seek(282);
	bs >> numStarsFound;
}

/*
 * Returns true if ACS is on
 */
bool ACS::isACSOn(){
	return isOn;
}


