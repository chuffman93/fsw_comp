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

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "util/Logger.h"

#include <iostream>

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

ACPPacket * CDHCPUUsageHandler::Handle(const ACPPacket & packet){
	return (CDHCPUUsage());
}

ACPPacket * CDHMemUsageHandler::Handle(const ACPPacket & packet){
	return (CDHMemUsage());
}

ACPPacket * CDHTempStartHandler::Handle(const ACPPacket & packet){
	return (CDHTempStart());
}

ACPPacket * CDHTempReadHandler::Handle(const ACPPacket & packet){
	return (CDHTempRead());
}

ACPPacket * CDHHotSwapsHandler::Handle(const ACPPacket & packet){
	return (CDHHotSwaps());
}

ACPPacket * CDHPowerMonitorsHandler::Handle(const ACPPacket & packet){
	return (CDHPowerMonitors());
}

ACPPacket * CDHStartPMHandler::Handle(const ACPPacket & packet){
	return (CDHStartPM());
}

ACPPacket * CDHCleanFSHandler::Handle(const ACPPacket & packet){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_DEBUG, "CDHHandlers: CleanFilesHandler entered");

	uint16 current_week = 50; // TODO: gpsServer->GetWeek();

	uint8 * msgPtr = packet.getMessageBuff();
	if(msgPtr == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "CDHHandlers: NULL msg");
		ACPPacket * ret = new ACPPacket(CDH_CLEAN_FS_FAILURE);
		return ret;
	}

	uint32 outputArray[2];
	for(uint8 i = 0; i < 2; i++){
		outputArray[i] = GetUInt(msgPtr);
		msgPtr += 4;
	}

	if((outputArray[0] >= current_week) || (outputArray[1] >= current_week) || (outputArray[0] > outputArray[1])){
		logger->Log(LOGGER_LEVEL_WARN, "CDHHandlers: bad weeks");
		ACPPacket * ret = new ACPPacket(CDH_CLEAN_FS_FAILURE);
		return ret;
	}

	return (CleanFiles(outputArray[0], outputArray[1]));
}
