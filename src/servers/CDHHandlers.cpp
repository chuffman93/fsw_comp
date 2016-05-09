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

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include <iostream>

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

ReturnMessage * CDHCPUUsageHandler::Handle(const FSWPacket & packet)
{
	return (CDHCPUUsage());
}

ReturnMessage * CDHMemUsageHandler::Handle(const FSWPacket & packet)
{
	return (CDHMemUsage());
}

ReturnMessage* CDHStorageHandler::Handle(const FSWPacket & packet)
{
	return (CDHStorage());
}

ReturnMessage* CDHTempBusHandler::Handle(const FSWPacket & packet)
{
	cout<<"CPU Temp Bus Handler(): Made it in"<<endl;
	return (CDHTempBus());
}

ReturnMessage* CDHHotSwapsHandler::Handle(const FSWPacket & packet)
{
	cout<<"CPU Hot Swaps Handler(): Made it in"<<endl;
	return (CDHHotSwaps());
}

// TODO: CPU_USAGE, MEM_USAGE, STORAGE, TEMP, HOT_SWAP
