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

// called from CDH server
void prepPowerGPIOs(void);
void CDHSystemInfo(void);
void CDHTempStart(void);
void CDHTempRead(float * tempArray);
void toggleResetLine(HardwareLocationIDType subsystem, bool state);
void toggleSubPower(HardwareLocationIDType subsystem, bool state);

// called from functions above
bool StartTempSensor(int bus, int sensor);
float ReadTempSensor(int bus, int sensor);
}
}

#endif /* CDHSTDTASKS_H_ */
