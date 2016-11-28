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
		logger->Log(LOGGER_LEVEL_WARN, "COMStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "COM LED Toggle successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "COM LED Toggle failed");
		return false;
	}
}

bool COMBlinkRate(uint16 rate){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "COMStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "COM LED set rate successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "COM LED set rate failed");
		return false;
	}
}

int COMLEDData(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "COMStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != 3){
		logger->Log(LOGGER_LEVEL_WARN, "COMStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "COMStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Log(LOGGER_LEVEL_DEBUG, "COM LED power state: %u", powerStatus);
		logger->Log(LOGGER_LEVEL_DEBUG, "COM LED blink rate:  %u", result);
		return (powerStatus + result);
	}
}

//Diagnostic
bool COMTestAlive(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "COM is alive");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "COM is not alive");
		return false;
	}
}

// COM Specific
bool COMSimplex(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "COM: commanding COM into simplex");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_SIMPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Log(LOGGER_LEVEL_WARN, "COM: Error entering Simplex");
	}

	return(response->isSuccess());
}

bool COMHalfDuplex(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "COM: commanding COM into half duplex");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HALF_DUPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Log(LOGGER_LEVEL_WARN, "COM: Error entering Half Duplex");
	}

	return(response->isSuccess());
}

bool COMFullDuplex(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "COM: commanding COM into full duplex");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_FULL_DUPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Log(LOGGER_LEVEL_WARN, "COM: Error entering full duplex");
	}

	return(response->isSuccess());
}

//ACPPacket * COMHealthStatus(void){
//	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HS_CMD);
//	ACPPacket * HSRet = DispatchPacket(HSQuery);
//
//	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//	logger->Log(LOGGER_LEVEL_FATAL, "COMStdTasks: unfinished function entered!");
//
//	return HSRet;
//}

// Non-opcode tasks
bool COMSelfCheck(){
	if(!COMToggleLED(true))
		return false;
	usleep(1000000);
	if(!COMBlinkRate(1000))
		return false;
	usleep(1000000);
	if(!(COMLEDData() == 1001))
		return false;
	return true;
}

}
}
