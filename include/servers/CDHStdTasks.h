/*
 * CDHStdTasks.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#ifndef CDHSTDTASKS_H_
#define CDHSTDTASKS_H_

#include "core/StdTypes.h"

namespace AllStar{
namespace Servers{

AllStar::Core::ACPPacket * CDHCPUUsage(void);
AllStar::Core::ACPPacket * CDHMemUsage(void);
AllStar::Core::ACPPacket * CDHTempStart(void);
AllStar::Core::ACPPacket * CDHTempRead(void);
AllStar::Core::ACPPacket * CDHHotSwaps(void);
AllStar::Core::ACPPacket * CDHPowerMonitors(void);
AllStar::Core::ACPPacket * CDHStartPM(void);
AllStar::Core::ACPPacket * CleanFiles(uint16 weekStart, uint16 weekEnd);
bool StartTempSensor(int bus, int sensor);
void toggleResetLine(HardwareLocationIDType subsystem, bool state);
float ReadTempSensor(int bus, int sensor);
void prepPowerGPIOs(void);
void toggleSubPower(HardwareLocationIDType subsystem, bool state);

}
}

#endif /* CDHSTDTASKS_H_ */
