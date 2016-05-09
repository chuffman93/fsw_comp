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

#include <iostream>

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 CMDSwitchProtocolHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT};

ReturnMessage * CMDSwitchProtocolHandler::Handle(const FSWPacket & packet)
{
	cout<<"CHECK 1 ------------------------------------------------------------------------"<<endl;
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(CMD_ACP_SWITCH_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	uint32 subsystem = * (uint32 *) outputArray[0];
	uint32 protocol = * (uint32 *) outputArray[1];


	// Error Checking
	if((subsystem < HARDWARE_LOCATION_MIN) || (subsystem >= HARDWARE_LOCATION_MAX))
	{
		ErrorMessage err(CMD_ACP_SWITCH_BAD_SUBSYS);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	if((protocol < ACP_PROTOCOL_MIN) || (protocol >= ACP_PROTOCOL_MAX))
	{
		ErrorMessage err(CMD_ACP_SWITCH_BAD_PROTOCOL);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}


	// Get CMDServer Instance, and change protocol for given subsystem
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	cmdServer->subsystem_acp_protocol[subsystem] = protocol;

	DataMessage msg(CMD_ACP_SWITCH_SUCCESS);
	ReturnMessage * ret = new ReturnMessage(&msg, true);
	return ret;
}
