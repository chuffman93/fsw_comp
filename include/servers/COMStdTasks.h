/*
 * 	COMStdTasks.h
 * 	Written by: Brian Campuzano
 * 	   Created: 10/20/2012
 *
 * 	Updated: Alex St. Clair
 */

#ifndef COMSTDTASKS_H_
#define COMSTDTASKS_H_

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "servers/SubsystemServer.h"
#include "servers/COMServer.h"

namespace AllStar{
namespace Servers{

// Debug
bool COMToggleLED(bool state);
bool COMBlinkRate(uint16 rate);
int COMLEDData();

// Standard
void COMPrepReset();

// Diagnostic
bool COMTestAlive();

// COM-specific
bool COMSimplex();
bool COMHalfDuplex();
bool COMFullDuplex();
void COMSerializeBeacon();
bool COMSendBeacon();

// Non-opcode standard tasks
bool COMSelfCheck();

}
}

#endif /* COMSTDTASKS_H_ */
