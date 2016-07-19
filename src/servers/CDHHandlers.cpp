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
