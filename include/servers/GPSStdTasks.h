/*
 * GPSStdTasks.h
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef GPSSTDTASKS_H_
#define GPSSTDTASKS_H_

#include "core/ACPPacket.h"

using namespace AllStar::Core;

namespace AllStar {
namespace Servers {

bool BESTXYZProcess(char * buffer,const size_t size);
uint32 CRCValue_GPS(int i);
uint32 CalculateCRC_GPS(char * buffer);
void UpdateTime(uint16 GPSWeek, float GPSSec);
void ConvertToEpochTime(uint16 GPSWeek, float GPSSec, uint32 * secondsFromEpoch);
void ConvertToGPSTime(uint32 currTimeSec, uint16 * GPSWeek, float * GPSSec);

}
}

#endif /* GPSSTDTASKS_H_ */
