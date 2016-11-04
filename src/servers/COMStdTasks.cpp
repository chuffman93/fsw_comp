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

	if(ret->isSuccess()){
		logger->Log("COM LED Toggle successful", LOGGER_LEVEL_INFO);
		return true;
	}else{
		logger->Log("COM LED Toggle failed", LOGGER_LEVEL_WARN);
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
		logger->Log("COMStdTasks: NULL blink rate return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->isSuccess()){
		logger->Log("COM LED set rate successful", LOGGER_LEVEL_INFO);
		return true;
	}else{
		logger->Log("COM LED set rate failed", LOGGER_LEVEL_WARN);
		return false;
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

	if(ret->getLength() != 3){
		logger->Log("COMStdTasks: Incorrect LED data return length", LOGGER_LEVEL_WARN);
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log("COMStdTasks: NULL LED Data message buffer", LOGGER_LEVEL_WARN);
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Log("COM LED power state: %u", powerStatus, LOGGER_LEVEL_INFO);
		logger->Log("COM LED blink rate:  %u", result, LOGGER_LEVEL_INFO);
		return true;
	}
}

//Diagnostic
bool COMTestAlive(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		logger->Log("COM is alive", LOGGER_LEVEL_INFO);
		return true;
	}else{
		logger->Log("COM is not alive", LOGGER_LEVEL_FATAL);
		return false;
	}
}

// COM Specific
bool COMSimplex(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("COM: commanding COM into simplex", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_SIMPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Log("COM: Error entering Simplex", LOGGER_LEVEL_WARN);
	}

	return(response->isSuccess());
}

bool COMHalfDuplex(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("COM: commanding COM into half duplex", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HALF_DUPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Log("COM: Error entering Half Duplex", LOGGER_LEVEL_WARN);
	}

	return(response->isSuccess());
}

bool COMFullDuplex(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("COM: commanding COM into full duplex", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_FULL_DUPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Log("COM: Error entering full duplex", LOGGER_LEVEL_WARN);
	}

	return(response->isSuccess());
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
