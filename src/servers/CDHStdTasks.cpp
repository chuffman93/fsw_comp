/*
 * CDHStdTasks.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"

#include "core/ModeManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/DataMessage.h"
#include "core/Dispatcher.h"

#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		ReturnMessage * CDHUsage(void)
		{
			//Actual usage code goes here------------------------------
			float usage = 22.0;
			//---------------------------------------------------------

			VariableTypeData usageHold(usage);

			list<VariableTypeData *> params;
			params.push_back(&usageHold);

			DataMessage dat(CDH_USAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		// TODO: Add Temperature Bus, Hot swaps, storage, storage management
	}
}
