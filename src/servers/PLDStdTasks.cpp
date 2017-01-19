/*
 * PLDStdTasks.cpp
 * Written by: Caitlyn Cooke
 * Created: 12/1/12
 */

#include "servers/PLDStdTasks.h"
#include "servers/PLDServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <stdio.h>
#include <string.h>
#include <cmath>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACPPacket * PLDHealthStatus(void)
{
	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);
	return HSRet;
}

// Standard
void PLDPrepReset(){
	ACPPacket * cmd = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, SUBSYSTEM_RESET_CMD);
	ACPPacket * ret = DispatchPacket(cmd); // no need to check the return, we're restarting anyway
}

} // end namespace servers
} // end namespace AllStar
