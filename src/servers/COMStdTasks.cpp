/*
 * COMStdTasks.cpp
 * Written by: Brian Campuzano
 * Created: 10/20/2012
 */

#include "servers/COMStdTasks.h"
#include "servers/COMServer.h"
#include "servers/CMDServer.h"
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

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_ON_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("COMStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Debug("COM LED Toggle successful");
		return true;
	}else{
		logger->Warning("COM LED Toggle failed");
		return false;
	}
}

bool COMBlinkRate(uint16 rate){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("COMStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Debug("COM LED set rate successful");
		return true;
	}else{
		logger->Warning("COM LED set rate failed");
		return false;
	}
}

int COMLEDData(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Warning("COMStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != 3){
		logger->Warning("COMStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Warning("COMStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Debug("COM LED power state: %u", powerStatus);
		logger->Debug("COM LED blink rate:  %u", result);
		return (powerStatus + result);
	}
}

// Standard
void COMPrepReset(){
	ACPPacket * cmd = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, SUBSYSTEM_RESET_CMD);
	ACPPacket * ret = DispatchPacket(cmd); // no need to check the return, we're restarting anyway
}

// Diagnostic
bool COMTestAlive(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		return true;
	}else{
		return false;
	}
}

// COM Specific
bool COMSimplex(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("COM: commanding COM into simplex");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_SIMPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Warning("COM: Error entering Simplex");
	}

	return(response->isSuccess());
}

bool COMHalfDuplex(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("COM: commanding COM into half duplex");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HALF_DUPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Warning("COM: Error entering Half Duplex");
	}

	return(response->isSuccess());
}

bool COMFullDuplex(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("COM: commanding COM into full duplex");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_FULL_DUPLEX);
	ACPPacket * response = DispatchPacket(command);

	if(!response->isSuccess()){
		logger->Warning("COM: Error entering full duplex");
	}

	return(response->isSuccess());
}

bool COMSendBeacon(){
	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("COM: sending beacon");

	uint8 * buffer = new uint8[BeaconStruct::size];
	cmdServer->serializeBeacon(buffer);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_BEACON, BeaconStruct::size, buffer);
	ACPPacket * response = DispatchPacket(command);

	return(response->isSuccess());
}

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
