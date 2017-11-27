/*
 * PLDStdTasks.cpp
 * Written by: Caitlyn Cooke
 * Created: 12/1/12
 */

#include "servers/PLDStdTasks.h"
#include "servers/PLDServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FileSystem.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include <stdio.h>
#include <string.h>
#include <cmath>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACPPacket * PLDHealthStatus(void)
{
	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);
	return HSRet;
}

// Debug
bool PLDToggleLED(bool state){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * buffer = (uint8 *) malloc(sizeof(uint8));
	uint8 stateOut;
	if(state){
		stateOut = 1;
		memcpy(buffer, &stateOut, 1);
	}else{
		stateOut = 0;
		memcpy(buffer, &stateOut, 1);
	}

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, LED_ON_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("PLDStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Debug("PLD LED Toggle successful");
		return true;
	}else{
		logger->Warning("PLD LED Toggle failed");
		return false;
	}
}

bool PLDBlinkRate(uint16 rate){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("PLDStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Debug("PLD LED set rate successful");
		return true;
	}else{
		logger->Warning("PLD LED set rate failed");
		return false;
	}
}

int PLDLEDData(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("PLDStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != 3){
		logger->Warning("PLDStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Warning("PLDStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Debug("PLD LED power state: %u", powerStatus);
		logger->Debug("PLD LED blink rate:  %u", result);
		return (powerStatus + result);
	}
}

// Diagnostic
bool PLDTestAlive(){
	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);
	if(ret->isSuccess()){
		return true;
	}else{
		return false;
	}
}

// Standard
void PLDPrepReset(){
	ACPPacket * cmd = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, SUBSYSTEM_RESET_CMD);
	ACPPacket * ret = DispatchPacket(cmd); // no need to check the return, we're restarting anyway
}

// Non-opcode tasks
bool PLDSelfCheck(){
	if(!PLDToggleLED(true))
		return false;
	usleep(1000000);
	if(!PLDBlinkRate(1000))
		return false;
	usleep(1000000);
	if(!(PLDLEDData() == 1001))
		return false;
	return true;
}

uint16 PLDUpdateDataNumber() {
	uint16 dataNumber;
	FILE * fp;
	uint8 buffer[2];
	bool success = true;

	// read the current value
	dataNumber = PLDReadDataNumber();
	dataNumber++;

	// write the updated value
	buffer[0] = (dataNumber & 0xFF00) >> 8;
	buffer[1] = (dataNumber & 0x00FF);
	fp = fopen(RAD_NUM_FILE, "wb");
	if (fp != NULL) {
		success &= (fputc(buffer[0], fp) != EOF);
		success &= (fputc(buffer[1], fp) != EOF);
		fclose(fp);
	}

	if (!success) {
		remove(RAD_NUM_FILE);
	}

	return dataNumber;
}

uint16 PLDReadDataNumber() {
	uint16 dataNumber = 1;
	FILE * fp;
	int char1, char2;
	bool success = true;

	if (access(RAD_NUM_FILE, F_OK) != -1) {
		fp = fopen(RAD_NUM_FILE, "rb");
		if (fp != NULL) {
			success &= (char1 = fgetc(fp)) != EOF;
			success &= (char2 = fgetc(fp)) != EOF;
			if (success) {
				dataNumber = ((uint16) char1) << 8 | ((uint16) char2);
			}
			fclose(fp);
		}
	}

	return dataNumber;
}

} // end namespace servers
} // end namespace AllStar
