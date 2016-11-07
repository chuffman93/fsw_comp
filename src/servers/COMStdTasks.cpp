/*
 * COMStdTasks.cpp
 * Written by: Brian Campuzano
 * Created: 10/20/2012
 */

#include "servers/COMStdTasks.h"
#include "servers/COMServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "POSIX.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

//ACPPacket * COMHealthStatus(void){
//	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HS_CMD);
//	ACPPacket * HSRet = DispatchPacket(HSQuery);
//
//	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//	logger->Log(LOGGER_LEVEL_FATAL, "COMStdTasks: unfinished function entered!");
//
//	return HSRet;
//}

}
}
