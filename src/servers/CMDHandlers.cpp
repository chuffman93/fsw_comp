/*
 * CMDHandlers.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: Alex St. Clair
 */

#include "servers/CMDHandlers.h"
#include "servers/CMDStdTasks.h"
#include "servers/CMDServer.h"
#include "servers/DispatchStdTasks.h"
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

ACPPacket * CMDSwitchProtocolHandler::Handle(const ACPPacket & packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_DEBUG, "CMDSwitchProtocolHandler: entered");

	uint8 * msgPtr = packet.getMessageBuff();
	if(msgPtr == NULL){
		ACPPacket * ret = new ACPPacket(CMD_ACP_SWITCH_FAILURE, false);
		return ret;
	}

	uint32 outputArray[2];
	for(uint8 i = 0; i < 2; i++){
		outputArray[i] = GetUInt(msgPtr);
		msgPtr += 4;
	}

	uint32 subsystem = outputArray[0];
	uint32 protocol = outputArray[1];

	// Error Checking
	if((subsystem < HARDWARE_LOCATION_MIN) || (subsystem >= HARDWARE_LOCATION_MAX)){
		logger->Log(LOGGER_LEVEL_DEBUG, "CMDSwitchProtocolHandler: Bad subsystem! Subsystem: %u", subsystem);
		ACPPacket * ret = new ACPPacket(CMD_ACP_SWITCH_BAD_SUBSYS, false);
		return ret;
	}

	if((protocol < ACP_PROTOCOL_MIN) || (protocol >= ACP_PROTOCOL_MAX)){
		logger->Log(LOGGER_LEVEL_DEBUG, "CMDSwitchProtocolHandler: Bad protocol!");
		ACPPacket * ret = new ACPPacket(CMD_ACP_SWITCH_BAD_PROTOCOL, false);
		return ret;
	}


	// Get CMDServer Instance, and change protocol for given subsystem
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	cmdServer->subsystem_acp_protocol[subsystem] = protocol;

	logger->Log(LOGGER_LEVEL_DEBUG, "CMDSwitchProtocolHandler: success");
	ACPPacket * ret = new ACPPacket(CMD_ACP_SWITCH_SUCCESS, true);
	return ret;
}
