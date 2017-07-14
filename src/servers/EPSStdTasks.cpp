/*
 * EPSStdTasks.cpp
 *
 *	Written by: Conrad Hougen
 * 		Created: 7/10/12
 *
 * 	Updated: Alex St. Clair
 */

#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/COMServer.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

using namespace AllStar::Core;
using namespace AllStar::Servers;

namespace AllStar{
namespace Servers{

// Debug
bool EPSToggleLED(bool state){
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

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, LED_ON_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("EPSStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Debug("EPS LED Toggle successful");
		return true;
	}else{
		logger->Warning("EPS LED Toggle failed");
		return false;
	}
}

bool EPSBlinkRate(uint16 rate){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("EPSStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Debug("EPS LED set rate successful");
		return true;
	}else{
		logger->Warning("EPS LED set rate failed");
		return false;
	}
}

int EPSLEDData(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("EPSStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != 3){
		logger->Warning("EPSStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Warning("EPSStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Debug("EPS LED power state: %u", powerStatus);
		logger->Debug("EPS LED blink rate:  %u", result);
		return (powerStatus + result);
	}
}

// Diagnostic
bool EPSTestAlive(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		logger->Debug("EPS is alive");
		return true;
	}else{
		logger->Fatal("EPS is not alive");
		return false;
	}
}

// Command/Data
void EPSPowerCycle()
{
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Fatal("Sending EPS power cycle!");
	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, SUBSYSTEM_RESET_CMD);
	ACPPacket * response = DispatchPacket(query);

	usleep(5000000);

	// if we are here then the power cycle likely didn't happen
	logger->Fatal("Power didn't cycle!");
	// TODO: add more handling here!
}

// Non-opcode tasks
bool EPSSelfCheck(){
	if(!EPSToggleLED(true))
		return false;
	usleep(1000000);
	if(!EPSBlinkRate(1000))
		return false;
	usleep(1000000);
	if(!(EPSLEDData() == 1001))
		return false;
	return true;
}

} // End namespace Servers
} // End namespace AllStar
