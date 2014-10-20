/*
 * THMStdTasks.h
 * A collection of functions to be run in THMSubsystemLoop
 *
 * Written by: Caitlyn
 * Created: 12/1/12
 */

#ifndef THMSTDTASKS_H_
#define THMSTDTASKS_H_



#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "servers/SubsystemServer.h"
#include "servers/THMServer.h"

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::ReturnMessage * DataCollect(uint32 busNumber);
		void IsDark(void);		  
	}
}

#endif /* THMSTDTASKS_H_ */
