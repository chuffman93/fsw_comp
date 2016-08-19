/*
 * EPSHandlers.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *  Updated on: 10/26/2012
 *  	Author: Gabrielle Massone
 *  Updated:
 *  	Author: Alex St. Clair
 */

#include "servers/EPSHandlers.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/StdTypes.h"
#include "core/ACPPacket.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"

using namespace AllStar::Core;
using namespace AllStar::Servers;

ACPPacket * EPSHSHandler::Handle(const ACPPacket & packet)
{
	return(EPSHealthStat());
}

ACPPacket * EPSPowerCycleHandler::Handle(const ACPPacket & packet)
{
	return(EPSPowerCycle());
}
