/*
 * CDHHandlers.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHHandlers.h"
#include "servers/CDHStdTasks.h"
#include "servers/CDHServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/GPSServer.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "util/Logger.h"

#include <iostream>

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

FSWPacket * CDHCPUUsageHandler::Handle(const FSWPacket & packet)
{
	return (CDHCPUUsage());
}

FSWPacket * CDHMemUsageHandler::Handle(const FSWPacket & packet)
{
	return (CDHMemUsage());
}

FSWPacket * CDHTempStartHandler::Handle(const FSWPacket & packet)
{
	return (CDHTempStart());
}

FSWPacket * CDHTempReadHandler::Handle(const FSWPacket & packet)
{
	return (CDHTempRead());
}

FSWPacket * CDHHotSwapsHandler::Handle(const FSWPacket & packet)
{
	return (CDHHotSwaps());
}

FSWPacket * CDHPowerMonitorsHandler::Handle(const FSWPacket & packet)
{
	return (CDHPowerMonitors());
}

FSWPacket * CDHStartPMHandler::Handle(const FSWPacket & packet)
{
	return (CDHStartPM());
}

FSWPacket * CDHCleanFSHandler::Handle(const FSWPacket & packet)
{
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("CDHHandlers: CleanFilesHandler entered", LOGGER_LEVEL_DEBUG);

	uint16 current_week = 50;//gpsServer->GetWeek();

	uint8 * msgPtr = packet.GetMessageBufPtr();
	if(msgPtr == NULL){
		logger->Log("CDHHandlers: NULL msg", LOGGER_LEVEL_WARN);
		FSWPacket * ret = new FSWPacket(CDH_CLEAN_FS_FAILURE, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}

	uint32 outputArray[2];
	for(uint8 i = 0; i < 2; i++){
		outputArray[i] = GetUInt(msgPtr);
		msgPtr += 4;
	}

	printf("Week start: %lu\n", outputArray[0]);
	printf("Week end:   %lu\n", outputArray[1]);

	if((outputArray[0] >= current_week) || (outputArray[1] >= current_week) || (outputArray[0] > outputArray[1])){
		logger->Log("CDHHandlers: bad weeks", LOGGER_LEVEL_WARN);
		FSWPacket * ret = new FSWPacket(CDH_CLEAN_FS_FAILURE, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}



	return (CleanFiles(outputArray[0], outputArray[1]));
}
