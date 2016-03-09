/*
 * CDHHandlers.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: alex
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

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

ReturnMessage * CDHUsageHandler::Handle(const FSWPacket & packet)
{
	return (CDHUsage());
}
