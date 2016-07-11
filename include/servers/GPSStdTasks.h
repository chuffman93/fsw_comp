/*
 * GPSStdTasks.h
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef GPSSTDTASKS_H_
#define GPSSTDTASKS_H_

#include "core/ReturnMessage.h"

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::FSWPacket * GPSHealthStatus();
		Phoenix::Core::FSWPacket * GPSTime();
		Phoenix::Core::FSWPacket * GPSPostion();
		bool GPSConversion();
		Phoenix::Core::FSWPacket * GPSReset();
		bool BESTXYZProcess(char * buffer,const size_t size);
		bool GPRMCProcess(char * buffer, const size_t size);
		double nmea_to_deg(char *nmea);
	}
}

#endif /* GPSSTDTASKS_H_ */
