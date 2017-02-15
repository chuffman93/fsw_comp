/*
 * ACSStdTasks.h
 * A collection of functions to be run in ACSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef ACSSTDTASKS_H_
#define ACSSTDTASKS_H_

#include "core/ACPPacket.h"
#include "core/StdTypes.h"

using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// Debug
bool ACSToggleLED(bool state);
bool ACSBlinkRate(uint16 rate);
int ACSLEDData();

// Standard
void ACSPrepReset();

// Diagnostic
bool ACSTestAlive();

// Command/Data
void ACSSendGPS();
bool ACSPointSun();
bool ACSPointNadir();
bool ACSPointGND();
bool ACSPointDest();

// Non-opcode tasks
bool ACSSelfCheck();

}
}

#endif /* ACSSTDTASKS_H_ */
