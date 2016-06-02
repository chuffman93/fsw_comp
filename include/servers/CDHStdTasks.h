/*
 * CDHStdTasks.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#ifndef CDHSTDTASKS_H_
#define CDHSTDTASKS_H_

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::ReturnMessage * CDHCPUUsage(void);
		Phoenix::Core::ReturnMessage * CDHMemUsage(void);
		Phoenix::Core::ReturnMessage * CDHStorage(void);
		Phoenix::Core::ReturnMessage * CDHTempStart(void);
		Phoenix::Core::ReturnMessage * CDHTempRead(void);
		Phoenix::Core::ReturnMessage * CDHHotSwaps(void);
		Phoenix::Core::ReturnMessage * CDHPowerMonitors(void);
		Phoenix::Core::ReturnMessage * CDHStartPM(void);
		bool StartSensor(int bus, int sensor);
		float ReadSensor(int bus, int sensor);
	}
}


#endif /* CDHSTDTASKS_H_ */
