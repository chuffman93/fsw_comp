/*
 * PLDStdTasks.cpp
 * Written by: Caitlyn Cooke
 * Created: 12/1/12
 */

#include "servers/PLDStdTasks.h"
#include "servers/PLDServer.h"
#include "servers/DispatchStdTasks.h"
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

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
		logger->Log(LOGGER_LEVEL_WARN, "PLDStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "PLD LED Toggle successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "PLD LED Toggle failed");
		return false;
	}
}

bool PLDBlinkRate(uint16 rate){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "PLDStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "PLD LED set rate successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "PLD LED set rate failed");
		return false;
	}
}

int PLDLEDData(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "PLDStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != 3){
		logger->Log(LOGGER_LEVEL_WARN, "PLDStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "PLDStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Log(LOGGER_LEVEL_DEBUG, "PLD LED power state: %u", powerStatus);
		logger->Log(LOGGER_LEVEL_DEBUG, "PLD LED blink rate:  %u", result);
		return (powerStatus + result);
	}
}

// Diagnostic
bool PLDTestAlive(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "PLD is alive");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "PLD is not alive");
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

} // end namespace servers
} // end namespace AllStar
