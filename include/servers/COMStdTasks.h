/*
 * 	COMStdTasks.h
 * 	Written by: Brian Campuzano
 * 	   Created: 10/20/2012
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

// \brief Collects health and status from COM.
AllStar::Core::ACPPacket * COMHealthStatus(void);

}
}

#endif /* COMSTDTASKS_H_ */
