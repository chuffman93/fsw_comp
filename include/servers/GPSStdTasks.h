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
		Phoenix::Core::ReturnMessage * GPSHealthStatus();
		Phoenix::Core::ReturnMessage * GPSTime();
		Phoenix::Core::ReturnMessage * GPSPostion();
		bool GPSConversion();
		Phoenix::Core::ReturnMessage * GPSReset();
		void GPSDataProcess(char * buffer,const size_t size);
	}
}

#endif /* GPSSTDTASKS_H_ */
