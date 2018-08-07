/*
 * COM.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */
#include "subsystem/COM.h"


COM::COM(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "COM");
	health.fileSize = FileManager::MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY COM_PATH "/COM";
	killCom = false;
}

COM::~COM(){}

/*
 * Powers on com and launches the com daemon
 * Should remove the commented sending of opcodes/ dead code
 */
void COM::initialize(){
	//TODO: error handling
	Logger::Stream(LEVEL_INFO,tags) << "Initializing COM";
	subPower.powerOn();

	LockGuard l(lock);

	std::vector<uint8_t> buff;

//	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE,buff);
//	if (!isSuccess(OP_TESTALIVE,retPacket1)){
//		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: COM is not alive. Opcode Received: " << retPacket1.opcode;
//	}
//
//	ACPPacket retPacket2 = sendOpcode(OP_TESTLED,buff);
//	if (!isSuccess(OP_TESTLED,retPacket2)){
//		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test LED: COM is not alive. Opcode Received: " << retPacket2.opcode;
//	}
//
//	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG,buff);
//	if (!isSuccess(OP_TESTCONFIG,retPacket3)){
//		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Configurations: COM is not alive. Opcode Received: " << retPacket3.opcode;
//	}
	launchDaemon();
}

/*
 * Handles mode transitions
 */
void COM::handleMode(FSWMode transition){
	LockGuard l(lock);
	bool success;
	switch (transition){
	case Mode_Reset:
		success = resetCOM(); // we dont need this, this transition method should only need a lock
		break;
	default:
		//TODO: error handling
		break;
	}
}

/*
 * Inherited function, irrelevant to COM
 */
void COM::handleConfig(){}

/*
 * Method used in terminating the com daemon
 */
void COM::setKillCom(bool toKill){
	killCom = toKill;
}

bool COM::getKillCom(){
	return killCom;
}

/*
 * Inherited function, irrelevant to COM
 */
void COM::updateConfig(){}

/*
 * Inherited function, irrelevant to COM
 * We should delete the dead code
 */
void COM::getHealthStatus(){
//	LockGuard l(lock);
//	std::vector<uint8_t> buff;
//	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS, buff);
//	health.recordBytes(acpReturn.message);
}

//! Launches Com Daemon
void COM::launchDaemon(){
	if (!(FileManager::checkExistance(TX_FILE))){
		Logger::Stream(LEVEL_INFO,tags) << "Launching comm daemon";
		usleep(5*1000*1000);
		char* cm_da[] = {(char*)"/bin/comm_daemon_arm",(char*)"&",NULL};
		da.launchProcess(cm_da,FALSE);
	}else{
		Logger::Stream(LEVEL_INFO,tags) << "Transmission off, comm daemon not running";
	}

}

//TODO: Do we need this? .... we don't need this, should be deleted
void COM::sendBeacon(){}

//! This function should be deleted entirely
bool COM::resetCOM(){

//	//Logger::Stream(LEVEL_INFO,tags) << "Preparing COM for Reset";
//
//	//std::vector<uint8_t> buff;
//	//ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
//	//if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
//		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: Unable to reset subsystem. Opcode Received: " << retPacket.opcode;
//		return false;
//	}
//	return true;
}

//TODO: Do we need this? ... no .. this should be removed
void COM::changeBaudRate(uint32_t baudRate){

	//Logger::Stream(LEVEL_INFO,tags) << "Changing COM Baud Rate to " << baudRate;

	//ByteStream bs;
	//bs << baudRate;

	//ACPPacket retPacket = sendOpcode(OP_HEALTHSTATUS, bs.vec());

}

/*!
 * This function is inherited yet irrelevant (mostly), good to keep though because I don't know how removing it would
 * effect everything if a COM opcode was accidently requested from the ground
 * Handles sending opcodes
 * \param opcode to be sent
 * \param buffer to be sent if need be
 */
ACPPacket COM::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
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

/*
 * This function is inherited yet irrelevant (mostly), good to keep though because I don't know how removing it would
 * effect everything if a COM opcode was accidently requested from the ground
 * checks if the COM opcode was sent successfully
 */

bool COM::isSuccess(COMOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}
/*
 * This function is inherited yet irrelevant (mostly), good to keep though because I don't know how removing it would
 * effect everything if a COM opcode was accidently requested from the ground
 * check if the subsystem opcode was sent successfully
 */
bool COM::isSuccess(SubsystemOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}
