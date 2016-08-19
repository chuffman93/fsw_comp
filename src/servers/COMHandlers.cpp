/*
 * COMHandlers.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: Brian Campuzano
 *
 */

#include "core/ModeManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "servers/COMHandlers.h"
#include "servers/COMStdTasks.h"
#include "servers/COMServer.h"
#include "servers/DispatchStdTasks.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

ACPPacket * COMHSHandler::Handle(const ACPPacket & packet){
	return (COMHealthStatus());
}

/*	TODO:
 * 		- Send files
 * 		- Recieve files (interface with UFTP in CMD)
 * 		- Login/Logout?
 * 		- Beacon
 */
