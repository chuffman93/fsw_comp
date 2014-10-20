/*
 * SCHModePosition.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#include "servers/SCHModePosition.h"
#include "servers/DispatchStdTasks.h"
#include "servers/SCHServer.h"

#include "core/StdTypes.h"
#include "core/MultiDataMessage.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		
		SCHModePosition::SCHModePosition(SystemModeEnum mode, float range, float x, float y, float z, bool lessThan)
		{
			compareRange = range;
			posX = x;
			posY = y;
			posZ = z;
			targetMode = mode;
			shorterThanCalc = lessThan;
		}

		bool SCHModePosition::CheckForSwitch()
		{
			//debug_led_set_led(2, LED_TOGGLE);
			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_SCH, SERVER_LOCATION_GPS, 1, 0, MESSAGE_TYPE_COMMAND, GPS_POSITION_CMD);
			
			uint32 SCHPosEnumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
			void * outputArray[3] = {NULL};
			if(!ExtractParameters((*retMsg), SCHPosEnumArray, 3, outputArray))
			{
				delete retMsg;
				return false;
			}
			
			float currentX = * (float *) outputArray[0];
			float currentY = * (float *) outputArray[1];
			float currentZ = * (float *) outputArray[2];
			delete retMsg;
			
			return(InRange(compareRange, posX, posY, posZ, currentX, currentY, currentZ, shorterThanCalc));
		}
	}
}
