/*
 *  SCHEventPosition.cpp
 *
 *  Created on: May 22, 2013
 *      Author: Brian Campuzano
 */

#include "servers/SCHEventPosition.h"
#include "servers/DispatchStdTasks.h"
#include "servers/SCHServer.h"
#include "util/Logger.h"

#include "core/MultiDataMessage.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{

		SCHEventPosition::SCHEventPosition(ServerLocationIDEnum sub, uint32 op, float range, float x, float y, float z, bool lessThan)
		{
			compareRange = range;
			posX = x;
			posY = y;
			posZ = z;
			opcode = op;
			subsystem = sub;
			shorterThanCalc = lessThan;
		}

		bool SCHEventPosition::CheckForEvent()
		{
			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("SCHEventPosition: unfinished method entered!", LOGGER_LEVEL_FATAL);

			FSWPacket * query = new FSWPacket(SERVER_LOCATION_SCH, SERVER_LOCATION_GPS, 0, GPS_POSITION_CMD, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * ret = DispatchPacket(query);

			
//			uint32 SCHPosEnumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
//			void * outputArray[3] = {NULL};
//			if(!ExtractParameters((*retMsg), SCHPosEnumArray, 3, outputArray))
//			{
//				delete retMsg;
//				return false;
//			}
//
//			float currentX = * (float *) outputArray[0];
//			float currentY = * (float *) outputArray[1];
//			float currentZ = * (float *) outputArray[2];
//			delete retMsg;
//
//			return(InRange(compareRange, posX, posY, posZ, currentX, currentY, currentZ, shorterThanCalc));

			return false;
		}

	}
}
