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

namespace AllStar
{
	namespace Servers
	{
		AllStar::Core::FSWPacket * CDHCPUUsage(void);
		AllStar::Core::FSWPacket * CDHMemUsage(void);
		AllStar::Core::FSWPacket * CDHTempStart(void);
		AllStar::Core::FSWPacket * CDHTempRead(void);
		AllStar::Core::FSWPacket * CDHHotSwaps(void);
		AllStar::Core::FSWPacket * CDHPowerMonitors(void);
		AllStar::Core::FSWPacket * CDHStartPM(void);
		AllStar::Core::FSWPacket * CleanFiles(uint16 weekStart, uint16 weekEnd);
		bool StartTempSensor(int bus, int sensor);
		void toggleResetLine(HardwareLocationIDType subsystem, bool state);
		float ReadTempSensor(int bus, int sensor);
		void prepPowerGPIOs(void);
		void toggleSubPower(HardwareLocationIDType subsystem, bool state);
	}
}


#endif /* CDHSTDTASKS_H_ */
