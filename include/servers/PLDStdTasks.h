/*
 * PLDStdTasks.h
 * A collection of functions to be run in PLDSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef PLDSTDTASKS_H_
#define PLDSTDTASKS_H_

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "servers/PLDServer.h"
#include "servers/SubsystemServer.h"

namespace AllStar{
namespace Servers{

// Debug
bool PLDToggleLED(bool state);
bool PLDBlinkRate(uint16 rate);
int PLDLEDData();
bool PLDTestAlive();

AllStar::Core::ACPPacket * PLDHealthStatus(void);
void PLDPrepReset();

// Non-opcode standard tasks
bool PLDSelfCheck();
uint16 PLDUpdateDataNumber();
uint16 PLDReadDataNumber();

}
}

#endif /* PLDSTDTASKS_H_ */
