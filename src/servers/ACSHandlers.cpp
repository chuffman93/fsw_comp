/*
 * ACSHandlers.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *
 *  Modified by: Conrad Hougen
 *  Last modified: 5/31/12
 *
 *  Modified by: Caleb Lipscomb
 *  Last modified 10/17/12
 *
 *  Updated: Alex St. Clair
 */

#include "servers/ACSHandlers.h"
#include "servers/ACSStdTasks.h"
#include "servers/ACSServer.h"

#include "util/Logger.h"

using namespace AllStar::Core;
using namespace AllStar::Servers;

ACPPacket * ACSMeasurementHandler::Handle(const ACPPacket & packet){
	return (ACSHealthStatus());
}

ACPPacket * ACSGPSHandler::Handle(const ACPPacket & packet){
	return (ACSSendGPS());
}
