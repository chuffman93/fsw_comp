/*
 * SCHModeTime.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#include "servers/SCHModeTime.h"
#include "servers/DispatchStdTasks.h"
#include "servers/SCHServer.h"

#include "core/StdTypes.h"
#include "core/MultiDataMessage.h"

#include "HAL/RTC.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
		SCHModeTime::SCHModeTime(SystemModeEnum mode, int week, float sec)
		{
			compareWeek = week;
			compareSec = sec;
			targetMode = mode;
		}

		bool SCHModeTime::CheckForSwitch()
		{
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
