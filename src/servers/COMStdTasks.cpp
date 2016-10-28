/*
 * COMStdTasks.cpp
 * Written by: Brian Campuzano
 * Created: 10/20/2012
 */

#include "servers/COMStdTasks.h"
#include "servers/COMServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "POSIX.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// Debug
bool COMToggleLED(bool state){
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

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_ON_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("COMStdTasks: NULL LED Toggle return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->getLength() != sizeof(uint8)){
		logger->Log("COMStdTasks: Incorrect Toggle LED return length", LOGGER_LEVEL_WARN);
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log("COMStdTasks: NULL LED Toggle message buffer", LOGGER_LEVEL_WARN);
			return false;
		}

		uint8 result = GetUInt8(ret->getMessageBuff());
		if(result == 1){
			logger->Log("COM LED Toggle successful", LOGGER_LEVEL_INFO);
			return true;
		}else{
			logger->Log("COM LED Toggle failed", LOGGER_LEVEL_WARN);
			return false;
		}
	}
}

bool COMBlinkRate(uint16 rate){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("COMStdTasks: NULL blink rate return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->getLength() != sizeof(uint8)){
		logger->Log("COMStdTasks: Incorrect blink rate return length", LOGGER_LEVEL_WARN);
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log("COMStdTasks: NULL blink rate message buffer", LOGGER_LEVEL_WARN);
			return false;
		}

		uint8 result = GetUInt8(ret->getMessageBuff());
		if(result == 1){
			logger->Log("COM blink rate successful", LOGGER_LEVEL_INFO);
			return true;
		}else{
			logger->Log("COM blink rate failed", LOGGER_LEVEL_WARN);
			return false;
		}
	}
}

bool COMLEDData(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("COMStdTasks: NULL LED Data return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->getLength() != sizeof(uint16)){
		logger->Log("COMStdTasks: Incorrect LED data return length", LOGGER_LEVEL_WARN);
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log("COMStdTasks: NULL LED Data message buffer", LOGGER_LEVEL_WARN);
			return false;
		}

		uint16 result = GetUInt16(ret->getMessageBuff());
		logger->Log("LED blink rate: %u", result, LOGGER_LEVEL_INFO);
		return true;
	}
}


//ACPPacket * COMHealthStatus(void){
//	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HS_CMD);
//	ACPPacket * HSRet = DispatchPacket(HSQuery);
//
//	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//	logger->Log("COMStdTasks: unfinished function entered!", LOGGER_LEVEL_FATAL);
//
//	return HSRet;
//}

}
}
