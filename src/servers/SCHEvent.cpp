/*
 * SCHEventTime.cpp
 *
 *  Created on: May 22, 2013
 *      Author: Brian Campuzano
 */

#include "core/ReturnMessage.h"
#include "core/StdTypes.h"

#include "servers/SCHEvent.h"
#include "servers/DispatchStdTasks.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;

namespace Phoenix
{
    namespace Servers
    {

		bool SCHEvent::CheckGoToNextEvent()
		{
			if(this->CheckForEvent())
			{
				ReturnMessage * ret;

				if(subsystem == SERVER_LOCATION_PLD)
				{
					ret = DispatchPacket(SERVER_LOCATION_SCH, SERVER_LOCATION_PLD, 1, 0, MESSAGE_TYPE_COMMAND, this->opcode);
				}
				else if(subsystem == SERVER_LOCATION_ACS)
				{
					ret = DispatchPacket(SERVER_LOCATION_SCH, SERVER_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, this->opcode);
				}
				else
				{
					return false;
				}

				if(ret->GetSuccess())
				{
					delete ret;
					return true;
				}
				//TODO: Handle failure
				delete ret;
			}
			return false;
		}
    }
}
