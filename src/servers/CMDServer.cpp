/*
* CMDServer.cpp
*
*  Created on: Feb 12, 2013
*      Author: Caitlyn
*  Modified by:
*/

#include "servers/CMDHandlers.h"
#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#include "POSIX.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

//Message Handler for ACP Protocol Switches
static CMDSwitchProtocolHandler * cmdSwitchProtocolHandler;
int CMDServer::subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

CMDServer::CMDServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
{
	for(int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++){
		subsystem_acp_protocol[i] = ACP_PROTOCOL_SPI;
	}
}

CMDServer::~CMDServer()
{
	// Left Intentionally Blank
}

void CMDServer::Initialize(void)
{
	cmdSwitchProtocolHandler = new CMDSwitchProtocolHandler();

	// setup for uftp
	portSetup();
	uftpSetup();
}

#ifdef TEST
void CMDServer::Destroy(void)
{
	delete cmdSwitchProtocolHandler;
}
#endif

bool CMDServer::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized());
}

CMDServer & CMDServer::operator=(const CMDServer & source)
{
	if (this == &source)
	{
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void CMDServer::Update(SystemModeEnum mode)
{
	// Called by mode manager each time mode changes
	//* Ex: Needs to do things to close mode 1, enter mode 2
	//* Setup and tear down between modes
}

bool CMDServer::RegisterHandlers()
{
	bool success = true;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	// CMD Command Opcode
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH), cmdSwitchProtocolHandler);
	success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH), true);

	/*
	for(int opcode = EPS_SUB_ERROR_MIN; opcode < EPS_SUB_ERROR_MAX; opcode++)
	{
		success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), epsErrorHandler);
		success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
	}
	*/


	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

void CMDServer::SubsystemLoop(void)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log("CMDServer: Entered Subsystem Loop", LOGGER_LEVEL_INFO);

	while(1)
	{
		//while(Listen(id));
		uint64_t LastTimeTick = getTimeInMillis();

		printf("loop\n");

		waitUntil(LastTimeTick, 1000);

	}
}

} // End of namespace Servers
} // End of namespace AllStar
