/*
 * ACSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 *
 * Updated: Alex St. Clair
 */

#include "servers/ACSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/ACSServer.h"
#include "servers/GPSServer.h"

#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

namespace AllStar{
namespace Servers{

ACPPacket * ACSHealthStatus(void){
	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_HS_CMD);
	return(DispatchPacket(HSQuery));
}

ACPPacket * ACSSendGPS(){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACSStdTasks: ACSSendGPS(): Entered", LOGGER_LEVEL_DEBUG);

	uint8 * buffer = (uint8 *) malloc(6*sizeof(double)+sizeof(float)+sizeof(int));
	AddDouble(buffer, gpsServer->GetGPSDataPtr()->posX);
	AddDouble(buffer + 8, gpsServer->GetGPSDataPtr()->posY);
	AddDouble(buffer + 16, gpsServer->GetGPSDataPtr()->posZ);
	AddDouble(buffer + 24, gpsServer->GetGPSDataPtr()->velX);
	AddDouble(buffer + 32, gpsServer->GetGPSDataPtr()->velY);
	AddDouble(buffer + 40, gpsServer->GetGPSDataPtr()->velZ);
	AddUInt32(buffer + 48, gpsServer->GetGPSDataPtr()->GPSWeek);
	AddFloat(buffer + 52, gpsServer->GetGPSDataPtr()->GPSSec);

	ACPPacket * send = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_GPS_CMD, 6*sizeof(double)+sizeof(float)+sizeof(int), buffer);
	return(DispatchPacket(send));
}

}
}
