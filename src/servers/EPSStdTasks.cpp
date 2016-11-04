/*
 * EPSStdTasks.cpp
 *
 *	Written by: Conrad Hougen
 * 		Created: 7/10/12
 *
 * 	Updated: Alex St. Clair
 */

#include "HAL/RTC.h"

#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/COMServer.h"

#include "core/Singleton.h"
#include "core/Factory.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#include "HAL/Power.h"
#include "HAL/Interrupt.h"
#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

using namespace AllStar::Core;
using namespace AllStar::HAL;
using namespace AllStar::Servers;

namespace AllStar{
namespace Servers{

// Debug
bool EPSToggleLED(bool state){
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

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, LED_ON_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("EPSStdTasks: NULL LED Toggle return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->isSuccess()){
		logger->Log("EPS LED Toggle successful", LOGGER_LEVEL_INFO);
		return true;
	}else{
		logger->Log("EPS LED Toggle failed", LOGGER_LEVEL_WARN);
		return false;
	}
}

bool EPSBlinkRate(uint16 rate){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("EPSStdTasks: NULL blink rate return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->isSuccess()){
		logger->Log("EPS LED set rate successful", LOGGER_LEVEL_INFO);
		return true;
	}else{
		logger->Log("EPS LED set rate failed", LOGGER_LEVEL_WARN);
		return false;
	}
}

bool EPSLEDData(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("EPSStdTasks: NULL LED Data return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->getLength() != 3){
		logger->Log("EPSStdTasks: Incorrect LED data return length", LOGGER_LEVEL_WARN);
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log("EPSStdTasks: NULL LED Data message buffer", LOGGER_LEVEL_WARN);
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Log("EPS LED power state: %u", powerStatus, LOGGER_LEVEL_INFO);
		logger->Log("EPS LED blink rate:  %u", result, LOGGER_LEVEL_INFO);
		return true;
	}
}

// Diagnostic
bool EPSTestAlive(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		logger->Log("EPS is alive", LOGGER_LEVEL_INFO);
		return true;
	}else{
		logger->Log("EPS is not alive", LOGGER_LEVEL_FATAL);
		return false;
	}
}

// Command/Data
void EPSHealthStat()
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Log("EPSStdTasks: NULL HSRet", LOGGER_LEVEL_ERROR);
		return;
	}

	if(HSRet->getLength() != 8*sizeof(uint16)){
		logger->Log("EPSStdTasks: EPSHealthStat(): incorrect message length!", LOGGER_LEVEL_WARN);

		//TODO: return error?
		//PacketProcess(SERVER_LOCATION_EPS, HSRet);
		return;
	}else{
		logger->Log("EPSStdTasks: EPSHealthStat(): packet dispatched, HSRet acquired", LOGGER_LEVEL_INFO);
		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			//Error
			//PacketProcess(SERVER_LOCATION_EPS, HSRet);
			return;
		}

		uint16 outputArray[8];
		for(uint8 i = 0; i < 8; i++){
			outputArray[i] = GetUInt16(msgPtr);
			msgPtr += 2;
		}

		EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));

		epsServer->EPSState.current3v3		= outputArray[0];
		epsServer->EPSState.voltage3v3		= outputArray[1];
		epsServer->EPSState.currentBatt		= outputArray[2];
		epsServer->EPSState.voltageBatt		= outputArray[3];
		epsServer->EPSState.current12v		= outputArray[4];
		epsServer->EPSState.voltage12v		= outputArray[5];
		epsServer->EPSState.battStatus		= outputArray[6];
		epsServer->EPSState.stateOfCharge	= outputArray[7];

		logger->Log("EPS H&S: 3v3 Current:     %u", epsServer->EPSState.current3v3,    LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: 3v3 Voltage:     %u", epsServer->EPSState.voltage3v3,    LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: Batt Current:    %u", epsServer->EPSState.currentBatt,   LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: Batt Voltage:    %u", epsServer->EPSState.voltageBatt,   LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: 12v Current:     %u", epsServer->EPSState.current12v,    LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: 12v Voltage:     %u", epsServer->EPSState.voltage12v,    LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: Batt status:     %u", epsServer->EPSState.battStatus,    LOGGER_LEVEL_DEBUG);
		logger->Log("EPS H&S: State of charge: %u", epsServer->EPSState.stateOfCharge, LOGGER_LEVEL_DEBUG);

		//PacketProcess(SERVER_LOCATION_EPS, HSRet);
	}
}

void EPSPowerCycle()
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, SUBSYSTEM_RESET_CMD);
	ACPPacket * response = DispatchPacket(query);

	usleep(5000000);

	// if we are here then the power cycle likely didn't happen
	logger->Log("Power didn't cycle!", LOGGER_LEVEL_FATAL);
	// TODO: add more handling here!
}

} // End namespace Servers
} // End namespace AllStar
