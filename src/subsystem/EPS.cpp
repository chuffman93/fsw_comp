
/*
 * EPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */


#include "subsystem/EPS.h"
#include "core/FileManager.h"
#include "core/FileSystem.h"

#include <stdint.h>
#include <unistd.h>
#include <iostream>



EPS::EPS(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "EPS");
	health.fileSize = FileManager::MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY EPS_PATH "/EPS";
	batteryCharge = 0;
}

EPS::~EPS(){}

/*
 * Sends Test Alive, Test LED and Test Config opcodes to EPS
 */
void EPS::initialize(){
	//TODO: error handling

	Logger::Stream(LEVEL_INFO,tags) << "Initializing EPS";
	std::vector<uint8_t> buff;


	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
	if (!isSuccess(OP_TESTALIVE,retPacket1)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: EPS is not alive. Opcode Received: " << retPacket1.opcode;
	}

	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
	if (!isSuccess(OP_TESTLED,retPacket2)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test LED: EPS is not alive. Opcode Received: " << retPacket2.opcode;
	}

	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
	if (!isSuccess(OP_TESTCONFIG,retPacket3)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Configurations: EPS is not alive. Opcode Received: " << retPacket3.opcode;
	}
}

//! Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void EPS::handleMode(FSWMode transition){
	LockGuard l(lock);
	bool success;
	switch (transition) {
	case Mode_Reset:
		success = commandReset();
		break;
	default:
		break;
	}
}

/*
 * Inherited function, irrelevant for EPS
 */
void EPS::handleConfig(){}

/*
 * Inherited function, irrelevant for EPS
 */
void EPS::updateConfig(){}

//! Handles the capturing and storing of the health and status for a subsystem
void EPS::getHealthStatus(){

	LockGuard l(lock);
	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS, buff);

	health.recordBytes(acpReturn.message);

	ByteStream bs(acpReturn.message);
	bs.seek(12) >> batteryCharge;

	Logger::Stream(LEVEL_INFO, tags) << "Battery Charge: " << batteryCharge;
}

/*!
 * Handles the sending of opcodes
 * \param opcode to be sent
 * \param buffer to be sent if need be
 */
ACPPacket EPS::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
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

//! checks to see if the EPS opcode was sent successfully
bool EPS::isSuccess(EPSOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

//! checks if the subsystem opcode was sent successfully
bool EPS::isSuccess(SubsystemOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return true;
}


//! Power cycle the entire satellite
bool EPS::commandReset(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Reseting EPS";
	std::vector<uint8_t> buff;
	//getHealthStatus(); dead code, should be removed


	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
	if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: unable to reset EPS Opcode Received: " << retPacket.opcode;
	}

	sleep(10);

	subPower.reset();

	sleep(10);

	return false;
}

//! returns the battery charge
uint16_t EPS::getBatteryStateOfCharge(){
	return batteryCharge;
}

/*
 *  Dead code, should be removed
 */
//void EPS::getSleepTime(){
//	GPSPositionTime st = gps->pt;
//	uint32_t sleepTime = gps->calcSleepTime(st);
//	if(sleepTime > 0){
//		std::string writeToFile = "COM_MODE!\n";
//		std::vector<std::string> buff;
//		buff.push_back(writeToFile);
//		FileManager::writeToStringFile(COM_MODE,buff);
//		//Do not remove this delay or else Corey and Alex will be ;(
//		sleep(10);
//		ByteStream bs;
//		bs << sleepTime;
//		lock.lock();
//		ACPPacket retPacket = sendOpcode(OP_SLEEP,bs.vec());
//		lock.unlock();
//		if (!isSuccess(OP_SLEEP,retPacket)){
//			Logger::Stream(LEVEL_FATAL,tags) << "Time to sleep failed to send OpCode Returned: " << int(retPacket.opcode);
//		}else{
//			Logger::Stream(LEVEL_FATAL,tags) << "Sending Sleep Time: " << sleepTime;
//		}
//	}else{
//		Logger::Stream(LEVEL_INFO,tags) << "PolarCube is Within Range of Boulder and does not need to be sleeping";
//	}
//}

/*
 * Sends sleep time to EPS
 */
void EPS::sendSleepTime(uint32_t time){
	LockGuard l(lock);
	ByteStream bs;
	bs << time;
	sleep(10);
	Logger::Stream(LEVEL_FATAL,tags) << "Sending Sleep Time: " << time;
	ACPPacket retPacket = sendOpcode(OP_SLEEP,bs.vec());
	if (!isSuccess(OP_SLEEP,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Time to sleep failed to send OpCode Returned: " << int(retPacket.opcode);
	}
}
