/*
 * EPSStdTasks.h
 * A collection of functions to be run in EPSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef EPSSTDTASKS_H_
#define EPSSTDTASKS_H_

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "servers/PLDServer.h"
#include "servers/SubsystemServer.h"

namespace AllStar{
namespace Servers{

AllStar::Core::ACPPacket * PLDHealthStatus(void);

}
}

#endif /* EPSSTDTASKS_H_ */
