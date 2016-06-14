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

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "util/Logger.h"

#include <iostream>

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 CMDSwitchProtocolHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT};

FSWPacket * CMDSwitchProtocolHandler::Handle(const FSWPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("CMDHandlers: Unfinished handler (variable type)", LOGGER_LEVEL_FATAL);

	uint8 * msgPtr = packet.GetMessageBufPtr();
	if(msgPtr == NULL){
		FSWPacket * ret = new FSWPacket(0, CMD_ACP_SWITCH_FAILURE, false, true, MESSAGE_TYPE_ERROR);
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
	if((subsystem < HARDWARE_LOCATION_MIN) || (subsystem >= HARDWARE_LOCATION_MAX))
	{
		logger->Log("CMDSwitchProtocolHandler: Bad subsystem! Subsystem: %u", subsystem, LOGGER_LEVEL_DEBUG);
		FSWPacket * ret = new FSWPacket(0, CMD_ACP_SWITCH_BAD_SUBSYS, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}
	if((protocol < ACP_PROTOCOL_MIN) || (protocol >= ACP_PROTOCOL_MAX))
	{
		logger->Log("CMDSwitchProtocolHandler: Bad protocol!", LOGGER_LEVEL_DEBUG);
		FSWPacket * ret = new FSWPacket(0, CMD_ACP_SWITCH_BAD_PROTOCOL, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}


	// Get CMDServer Instance, and change protocol for given subsystem
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	cmdServer->subsystem_acp_protocol[subsystem] = protocol;

	FSWPacket * ret = new FSWPacket(0, CMD_ACP_SWITCH_SUCCESS, true, true, MESSAGE_TYPE_DATA);
	return ret;
}
