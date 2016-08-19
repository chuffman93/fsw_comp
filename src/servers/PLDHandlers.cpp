/*
 * PLDHandlers.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: Brian Campuzano
 */

#include "servers/PLDServer.h"
#include "servers/PLDHandlers.h"
#include "servers/PLDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/ACPPacket.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

ACPPacket * PLDHSHandler::Handle(const ACPPacket & packet)
{
	return (PLDHealthStatus());
}

