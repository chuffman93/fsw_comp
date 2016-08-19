/*
 * GPSStdTasks.h
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef GPSSTDTASKS_H_
#define GPSSTDTASKS_H_

#include "core/ACPPacket.h"

using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACPPacket * GPSHealthStatus();
ACPPacket * GPSTime();
ACPPacket * GPSPostion();
bool GPSConversion();
ACPPacket * GPSReset();
bool BESTXYZProcess(char * buffer,const size_t size);
bool GPRMCProcess(char * buffer, const size_t size);
double nmea_to_deg(char *nmea);
uint32 CRCValue_GPS(int i);
uint32 CalculateCRC_GPS(char * buffer);
uint8 CalculateNMEAChecksum(char * buffer);

}
}

#endif /* GPSSTDTASKS_H_ */
