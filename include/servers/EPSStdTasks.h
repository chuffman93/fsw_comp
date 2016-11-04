/*
 * EPSStdTasks.h
 * A collection of functions to be run in EPSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 *
 * Updated: Alex St. Clair
 * Updated: 10/2016
 */

#ifndef EPSSTDTASKS_H_
#define EPSSTDTASKS_H_

namespace AllStar{
namespace Servers{

// Debug
bool EPSToggleLED(bool state);
bool EPSBlinkRate(uint16 rate);
bool EPSLEDData();

// Diagnostic
bool EPSTestAlive();

// Command/Data
void EPSHealthStat();
void EPSPowerCycle();

}
}

#endif /* EPSSTDTASKS_H_ */
