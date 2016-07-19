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
		Phoenix::Core::FSWPacket * CDHCPUUsage(void);
		Phoenix::Core::FSWPacket * CDHMemUsage(void);
		Phoenix::Core::FSWPacket * CDHTempStart(void);
		Phoenix::Core::FSWPacket * CDHTempRead(void);
		Phoenix::Core::FSWPacket * CDHHotSwaps(void);
		Phoenix::Core::FSWPacket * CDHPowerMonitors(void);
		Phoenix::Core::FSWPacket * CDHStartPM(void);
		Phoenix::Core::FSWPacket * CleanFiles(uint16 weekStart, uint16 weekEnd);
		bool StartTempSensor(int bus, int sensor);
		void toggleResetLine(HardwareLocationIDType subsystem, bool state);
		float ReadTempSensor(int bus, int sensor);
		void prepPowerGPIOs(void);
		void toggleSubPower(HardwareLocationIDType subsystem, bool state);
	}
}


#endif /* CDHSTDTASKS_H_ */
