/*
 * EPSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */
#include "HAL/RTC.h"

#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/COMServer.h"

#include "core/ReturnMessage.h"
#include "core/DataMessage.h"
#include "core/CommandMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#include "HAL/Power.h"
#include "HAL/Interrupt.h"
#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;
using namespace Phoenix::HAL;
using namespace Phoenix::Servers;

namespace Phoenix{
namespace Servers{

FSWPacket * EPSHealthStat()
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("EPSStdTasks: unfinished function entered!", LOGGER_LEVEL_FATAL);

	FSWPacket * HSQuery = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, EPS_HS_CMD, true, false, MESSAGE_TYPE_COMMAND);
	FSWPacket * HSRet = DispatchPacket(HSQuery);
	logger->Log("EPSStdTasks: EPSHealthStat(): packet dispatched, HSRet acquired", LOGGER_LEVEL_INFO);
	if(HSRet == NULL){
		logger->Log("EPSStdTasks: Null HSRet", LOGGER_LEVEL_ERROR);
	}

	if(HSRet->GetMessageLength() != 6*sizeof(uint16)){
		logger->Log("EPSStdTasks: EPSHealthStat(): incorrect message length!", LOGGER_LEVEL_WARN);

		//TODO: return error?
		return HSRet;
	}else{
		// Parse buffer
		uint8 * msgPtr = HSRet->GetMessageBufPtr();
		if(msgPtr==NULL){
			//Error
			return HSRet;
		}

		uint16 outputArray[6];
		for(uint8 i = 0; i < 6; i++){
			outputArray[i] = GetUInt16(msgPtr);
			msgPtr += 2;
		}

		uint16 volt_3v3 	= outputArray[0];
		logger->Log("3v3  Voltage: %u", (uint32) volt_3v3, LOGGER_LEVEL_DEBUG);
		uint16 curr_3v3 	= outputArray[1];
		logger->Log("3v3  Current: %u", (uint32) curr_3v3, LOGGER_LEVEL_DEBUG);
		uint16 volt_vbat 	= outputArray[2];
		logger->Log("Vbat Voltage: %u", (uint32) volt_vbat, LOGGER_LEVEL_DEBUG);
		uint16 curr_vbat 	= outputArray[3];
		logger->Log("Vbat Current: %u", (uint32) curr_vbat, LOGGER_LEVEL_DEBUG);
		uint16 volt_12v 	= outputArray[4];
		logger->Log("12v  Voltage: %u", (uint32) volt_12v, LOGGER_LEVEL_DEBUG);
		uint16 curr_12v 	= outputArray[5];
		logger->Log("12v  Current: %u", (uint32) curr_12v, LOGGER_LEVEL_DEBUG);

		// Update beacon
		//COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
		//comServer -> UpdateEPSHS(battVoltage,battCurrent, stateOfCharge,battRemainingCapacity,cycleCount,batteryManagerStatus);
		return HSRet;
	}
}

FSWPacket * EPSPowerCycle()
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, EPS_POWER_CYCLE_CMD, true, false, MESSAGE_TYPE_COMMAND);
	FSWPacket * response = DispatchPacket(query);

	usleep(5000000);

	// if we are here then the power cycle likely didn't happen
	logger->Log("Power didn't cycle!", LOGGER_LEVEL_FATAL);
	// TODO: add more handling here!
}

FSWPacket * EPSStateOfCharge()
{
	FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, EPS_SOC_CMD, true, false, MESSAGE_TYPE_COMMAND);
	return(DispatchPacket(query));
}


FSWPacket * EPSDisableOCProt()
{
	FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, EPS_DISABLE_OC_PROT_CMD, true, false, MESSAGE_TYPE_COMMAND);
	return(DispatchPacket(query));
}


FSWPacket * EPSEnableOCProt()
{
	FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, EPS_ENABLE_OC_PROT_CMD, true, false, MESSAGE_TYPE_COMMAND);
	return(DispatchPacket(query));
}

} // End namespace Servers
} // End namespace Phoenix
