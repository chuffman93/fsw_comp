/*
 * SCHEventTime.cpp
 *
 *  Created on: May 22, 2013
 *      Author: Brian Campuzano
 */

#include "servers/SCHEventTime.h"
#include "servers/DispatchStdTasks.h"
#include "servers/SCHServer.h"

#include "core/ReturnMessage.h"
#include "core/MultiDataMessage.h"

//#include "boards/backplane/dbg_led.h"

#include "HAL/RTC.h"

using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{

		SCHEventTime::SCHEventTime(ServerLocationIDEnum sub, uint32 op, int week, float sec)
		{
			compareWeek = week;
			compareSec = sec;
			subsystem = sub;
			opcode = op;
		}

		bool SCHEventTime::CheckForEvent()
		{
			/*debug_led_set_led(2, LED_TOGGLE);*/
			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_SCH, SERVER_LOCATION_GPS, 1, 0, MESSAGE_TYPE_COMMAND, GPS_TIME_CMD);
			
			uint32 SCHTimeEnumArray[] = {VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_FLOAT};
			void * outputArray[2] = {NULL};
			if(!ExtractParameters((*retMsg), SCHTimeEnumArray, 2, outputArray))
			{
				return false;
			}
			
			int32 gpsWeek = * (int32 *) outputArray[0];
			//float gpsSec = * (float *) outputArray[1];
			
			uint32 seconds = 0;
			//TODO:RTC
			//RTCGetTime(&seconds, NULL);

			if(gpsWeek > compareWeek)
			{
				return true;
			}
			else if((gpsWeek == compareWeek) && (seconds >= compareSec))
			{
				return true;
			}
			return false;
		}
	}
}
