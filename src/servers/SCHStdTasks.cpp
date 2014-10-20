/*
 * SCHStdTasks.cpp
 *
 * Created: 11/7/2013 11:00:52 AM
 *  Author: cypa8074
 */ 

#include <list>
#include <float.h>

#include "servers/SCHStdTasks.h"
#include "servers/SCHServer.h"
#include "servers/SCHMode.h"
#include "servers/SCHModeTime.h"
#include "servers/SCHModePosition.h"
#include "servers/SCHEvent.h"
#include "servers/SCHEventTime.h"
#include "servers/SCHEventPosition.h"

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
		//build a schedule from file.
	//TODO FIXBUILD SCHEDULE FUNCTION
		ReturnMessage * BuildSchedule(const char * fileName)
		{
//			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
//			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
//
//			size_t bufferSize = -1;
//			//TODO::filehandler
//			//char * buffer = (char *) fileHandler->ReadFile(fileName, &bufferSize);
//
//			if(bufferSize <= 0)
//			{
//				ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//				ReturnMessage * ret = new ReturnMessage(&err, false);
//				return ret;
//			}
//
//			std::list<SCHMode *> * newSchedule = new std::list<SCHMode *>;
//
//			uint32 type = 0;
//			uint32 mode = 0;
//			float params[4] = {0};
//			volatile uint32 rv = 0;
//			uint32 i = 0;
//			double doubleHolder = 0;
//			char * bufferPtr;
//
//			bool lastNodeFlag = true;
//			while(lastNodeFlag && (i < SCHEDULE_QUEUE_MAXSIZE))
//			{
//				//removing sscanf with custom code to reduce code size.
//				//rv = sscanf(buffer, "%d,%d,%f,%f,%f,%f;%s", &type, &mode, &float1, &float2, &float3, &float4, buffer);
//
//				//extract mode
//				type = strtol(buffer, (&bufferPtr), 10);
//				if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, false);
//					return ret;
//				}
//				buffer = bufferPtr;
//				buffer++;
//
//				//extract mode
//				mode = strtol(buffer, (&bufferPtr), 10); //if we don't get a comma next the last value was not extracted correctly, return error.
//				if(bufferPtr[0] != ',')
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, false);
//					return ret;
//				}
//				buffer = bufferPtr;
//				buffer++;
//
//				//extract 4 float parameters
//				for(uint32 j = 0; j < 4; j++)
//				{
//					doubleHolder = strtod(buffer, &bufferPtr);
//					//if we are not extracting the 4th parameter
//					if((j != 3) && (bufferPtr[0] != ',')) //and we don't get a comma next the last value was not extracted correctly, return error.
//					{
//						//error stuff
//						ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//						ReturnMessage * ret = new ReturnMessage(&err, false);
//						return ret;
//					}
//					//if we are extracting the 4th parameter
//					if((j == 3) && (bufferPtr[0] != ';')) //and we don't get a semi-colon next the last value was not extracted correctly, return error.
//					{
//						//error stuff
//						ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//						ReturnMessage * ret = new ReturnMessage(&err, false);
//						return ret;
//					}
//					buffer = bufferPtr;
//					buffer++;
//					params[j] = (float) doubleHolder;
//				}
//
//				if(buffer[0] == ';') //ASCII value of ';'
//				{
//					// last node exit loop at end
//					lastNodeFlag = false;
//				}
//
//				if((type < SCH_ST_START) || (type >= SCH_ST_END))
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, false);
//					return ret;
//				}
//				//TODO: value error checking?
//				//if we get here should be a valid mode node.
//				if(type == SCH_ST_TIME)
//				{
//					newSchedule->push_back(new SCHModeTime(((SystemModeEnum) mode), (int)(params[0]), params[1]));
//				}
//				else if(type == SCH_ST_LOCATION)
//				{
//					newSchedule->push_back(new SCHModePosition(((SystemModeEnum) mode), params[0], params[1], params[2], params[3], true));
//				}
//			}
//
//			if(schServer->SetCurrentSchedule(newSchedule))
//			{
//				DataMessage msg(SCH_BUILD_SCHEDULE_SUCCESS);
//				ReturnMessage * ret = new ReturnMessage(&msg, true);
//				return ret;
//			}
//			else if(schServer->SetNextSchedule(newSchedule))
//			{
//				DataMessage msg(SCH_BUILD_SCHEDULE_SUCCESS);
//				ReturnMessage * ret = new ReturnMessage(&msg, true);
//				return ret;
//			}
//			ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//			ReturnMessage * ret = new ReturnMessage(&err, false);
//			return ret;
		}

		ReturnMessage * RunSchedule(void)
		{
			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
			if(schServer->RunCurrentSchedule())
			{
				//debug_led_set_led(0, LED_ON);
				DataMessage msg(SCH_RUN_SCHEDULE_SUCCESS);
				ReturnMessage * ret = new ReturnMessage(&msg, true);
				return ret;
			}
			ErrorMessage err(SCH_RUN_SCHEDULE_FAILURE);
			ReturnMessage * ret = new ReturnMessage(&err, false);
			return ret;
		}


		//build a schedule from file.
		//TODO::FIX BUILDPLDSCHEDULEFUNCTION
		ReturnMessage * BuildPLDSchedule(const char * fileName)
		{
//			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
//			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
//
//			size_t bufferSize = -1;
//			char * buffer = (char *) fileHandler->ReadFile(fileName, &bufferSize);
//
//			if(bufferSize <= 0)
//			{
//				ErrorMessage err(SCH_BUILD_PLD_SCHEDULE_FAILURE);
//				ReturnMessage * ret = new ReturnMessage(&err, true);
//				return ret;
//			}
//
//			std::list<SCHEvent *> * newPLDSchedule = new std::list<SCHEvent *>;
//
//			uint32 type = 0;
//			uint32 subsystem = 0;
//			uint32 opcode = 0;
//			float params[4] = {0};
//			volatile uint32 rv = 0;
//			uint32 i = 0;
//			double doubleHolder = 0;
//			char * bufferPtr;
//
//
//			bool lastNodeFlag = true;
//			while(lastNodeFlag && (i < SCHEDULE_QUEUE_MAXSIZE))
//			{
//
//				//removing sscanf with custom code to reduce code size.
//				//rv = sscanf(buffer, "%d,%d,%d,%f,%f,%f,%f;%s", &type, &subsystem, &opcode, &float1, &float2, &float3, &float4, buffer);
//
//				//extract mode
//				type = strtol(buffer, (&bufferPtr), 10);
//				if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, false);
//					return ret;
//				}
//				buffer = bufferPtr;
//				buffer++;
//
//				//extract subsystem
//				subsystem = strtol(buffer, (&bufferPtr), 10);
//				if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, false);
//					return ret;
//				}
//				buffer = bufferPtr;
//				buffer++;
//
//				//extract opcode
//				opcode = strtol(buffer, (&bufferPtr), 10);
//				if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, false);
//					return ret;
//				}
//				buffer = bufferPtr;
//				buffer++;
//
//				//extract 4 float parameters
//				for(uint32 j = 0; j < 4; j++)
//				{
//					doubleHolder = strtod(buffer, &bufferPtr);
//					//if we are not extracting the 4th parameter
//					if((j != 4) && (bufferPtr[0] != ',')) //and we don't get a comma next the last value was not extracted correctly, return error.
//					{
//						//error stuff
//						ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//						ReturnMessage * ret = new ReturnMessage(&err, false);
//						return ret;
//					}
//					//if we are extracting the 4th parameter
//					if((j == 4) && (bufferPtr[0] != ';')) //and we don't get a semi-colon next the last value was not extracted correctly, return error.
//					{
//						//error stuff
//						ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
//						ReturnMessage * ret = new ReturnMessage(&err, false);
//						return ret;
//					}
//					buffer = bufferPtr;
//					buffer++;
//					params[j] = (float) doubleHolder;
//				}
//
//				if(buffer[0] == ';') //ASCII value of ';'
//				{
//					// last node exit loop at end
//					lastNodeFlag = false;
//				}
//
//				if((type < SCH_ST_START) || (type >= SCH_ST_END))
//				{
//					//error stuff
//					ErrorMessage err(SCH_BUILD_PLD_SCHEDULE_FAILURE);
//					ReturnMessage * ret = new ReturnMessage(&err, true);
//					return ret;
//				}
//				//TODO: value error checking?
//				//if we get here should be a valid mode node.
//				if(type == SCH_ST_TIME)
//				{
//					newPLDSchedule->push_back(new SCHEventTime((ServerLocationIDEnum) subsystem,
//						opcode, (int)(params[0]), params[1]));
//				}
//				else if(type == SCH_ST_LOCATION)
//				{
//					newPLDSchedule->push_back(new SCHEventPosition((ServerLocationIDEnum) subsystem,
//						opcode, params[0], params[1], params[2], params[3], true));
//				}
//			}
//			if(schServer->SetCurrentPLDSchedule(newPLDSchedule))
//			{
//				DataMessage msg(SCH_BUILD_PLD_SCHEDULE_SUCCESS);
//				ReturnMessage * ret = new ReturnMessage(&msg, true);
//				return ret;
//			}
//			else if(schServer->SetNextPLDSchedule(newPLDSchedule))
//			{
//				DataMessage msg(SCH_BUILD_PLD_SCHEDULE_SUCCESS);
//				ReturnMessage * ret = new ReturnMessage(&msg, true);
//				return ret;
//			}
//			ErrorMessage err(SCH_BUILD_PLD_SCHEDULE_FAILURE);
//			ReturnMessage * ret = new ReturnMessage(&err, true);
//			return ret;
		}
		
		bool InRange(const float & compareRange, const float & targetX, const float & targetY, const float & targetZ,
			const float & currentX, const float & currentY, const float & currentZ, const bool & lessThanComparison)
		{
			//not using square roots to speed up the function
			float xDif = targetX - currentX;
			float yDif = targetY - currentY;
			float zDif = targetZ - currentZ;
			float newRange = (xDif*xDif) + (yDif*yDif) + (zDif*zDif);
			if(lessThanComparison)
			{
				if(newRange < (compareRange*compareRange))
				{
					return true;
				}
			}
			else
			{
				if(newRange > (compareRange*compareRange))
				{
					return true;
				}
			}
			
			return false;
		}
		
		ReturnMessage * SCHDefaultRange(const float & newRange)
		{
			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
			
			if(!schServer->SetNewDefaultRange(newRange))
			{
				ErrorMessage err(SCH_DEFAULT_RANGE_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, true);
				return ret;
			}
			DataMessage msg(SCH_DEFAULT_RANGE_SUCCESS);
			ReturnMessage * ret = new ReturnMessage(&msg, true);
			return ret;
		}
	}
}
