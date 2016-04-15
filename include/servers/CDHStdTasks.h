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

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::ReturnMessage * CDHCPUUsage(void);
		Phoenix::Core::ReturnMessage * CDHMemUsage(void);
		Phoenix::Core::ReturnMessage * CDHStorage(void);
		Phoenix::Core::ReturnMessage * CDHTempBus(void);
		Phoenix::Core::ReturnMessage * CDHHotSwaps(void);
	}
}


#endif /* CDHSTDTASKS_H_ */
