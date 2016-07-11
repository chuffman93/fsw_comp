/*
 * GPSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */
#include <string.h>
//#include <stdio.h>

#include "HAL/GPS.h"
#include "HAL/RTC.h"

#include "servers/GPSStdTasks.h"
#include "servers/GPSServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/ModeManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/DataMessage.h"
#include "core/ReturnMessage.h"
#include "core/VariableTypeData.h"
#include "core/CommandMessage.h"

#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
		uint32 GPSConversionEnumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, 
				VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
		
		FSWPacket * GPSHealthStatus()
		{
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				FSWPacket * ret = new FSWPacket(0, GPS_HS_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				FSWPacket * ret = new FSWPacket(0, GPS_HS_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}
			
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("GPSStdTasks: GPSHealth(): Unfinished Function (variable type)!", LOGGER_LEVEL_FATAL);
			
//			uint8 buff[350];
//			memcpy(buff, gpsData->header, 100);
//			memcpy((buff + 100), gpsData->message, 250);
//			DataMessage dat(GPS_HS_SUCCESS, VariableTypeData(buff, 350));
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//
//			//COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
//			//FIXME: figure out why eclipse doesn't like these lines
////			comServer -> UpdateGPSHS(GPSData->GPSWeek,GPSData->GPSSec,GPSData->posX,GPSData->posY,GPSData->posZ,
////				GPSData->velX,GPSData->velY,GPSData->velZ,GPSData->numSolution,GPSData->solAge);
//
//			return retMsg;
		}

		FSWPacket * GPSTime()
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("GPSStdTasks: GPSTime(): Unfinished Function (variable type)!", LOGGER_LEVEL_FATAL);

			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				FSWPacket * ret = new FSWPacket(0, GPS_TIME_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				FSWPacket * ret = new FSWPacket(0, GPS_TIME_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}

//			int32 week = gpsData->GPSWeek;
//			float sec = gpsData->GPSSec;
//			VariableTypeData weekHold(week);
//			VariableTypeData secHold(sec);
//
//			list<VariableTypeData *> params;
//			params.push_back(&weekHold);
//			params.push_back(&secHold);
//
//			DataMessage dat(GPS_TIME_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		FSWPacket * GPSPostion()
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("GPSStdTasks: GPSHealth(): Unfinished Function (variable type)!", LOGGER_LEVEL_FATAL);

			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				FSWPacket * ret = new FSWPacket(0, GPS_POSITION_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				FSWPacket * ret = new FSWPacket(0, GPS_POSITION_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}
			
//			VariableTypeData x_hold(gpsData->posX);
//			VariableTypeData y_hold(gpsData->posY);
//			VariableTypeData z_hold(gpsData->posZ);
//
//			list<VariableTypeData *> params;
//			params.push_back(&x_hold);
//			params.push_back(&y_hold);
//			params.push_back(&z_hold);
//
//			DataMessage dat(GPS_POSITION_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		//Talk to ACS to convert ECEF -> ECI
		bool GPSConversion()
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("GPSStdTasks: GPSConversion(): Unfinished Function (variable type)!", LOGGER_LEVEL_FATAL);

//			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
//			if (NULL == gpsServer)
//			{
//				ErrorMessage err(GPS_TIME_FAILURE);
//				ReturnMessage * eRet = new ReturnMessage(&err, false);
//				return eRet;
//			}
//			GPSData * gpsData = gpsServer->GetGPSDataPtr();
//			if(gpsData == NULL)
//			{
//				ErrorMessage err(GPS_POSITION_FAILURE);
//				ReturnMessage * eRet = new ReturnMessage(&err, false);
//				return eRet;
//			}
//
//			VariableTypeData posX_hold(gpsData->posX);
//			VariableTypeData posY_hold(gpsData->posY);
//			VariableTypeData posZ_hold(gpsData->posZ);
//			VariableTypeData velX_hold(gpsData->velX);
//			VariableTypeData velY_hold(gpsData->velY);
//			VariableTypeData velZ_hold(gpsData->velZ);
//			VariableTypeData sec_hold(gpsData->GPSSec);
//			VariableTypeData weekHold((uint32) gpsData->GPSWeek);
//
//			list<VariableTypeData *> params;
//			params.push_back(&posX_hold);
//			params.push_back(&posY_hold);
//			params.push_back(&posZ_hold);
//			params.push_back(&velX_hold);
//			params.push_back(&velY_hold);
//			params.push_back(&velZ_hold);
//			params.push_back(&sec_hold);
//
//			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_GPS, SERVER_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_GPS_CMD, params);
//			if(!ret->GetSuccess())
//			{
//				return false;
//			}
//
//			void * outputArray[6] = {NULL};
//			if(!ExtractParameters((*ret), GPSConversionEnumArray, 6, outputArray))
//			{
//				return false;
//			}
//
//			gpsData->posECIX= * (float *) outputArray[0];
//			gpsData->posECIY= * (float *) outputArray[1];
//			gpsData->posECIZ= * (float *) outputArray[2];
//			gpsData->velECIX= * (float *) outputArray[3];
//			gpsData->velECIY= * (float *) outputArray[4];
//			gpsData->velECIZ= * (float *) outputArray[5];
//
//			return true;
		}

		FSWPacket * GPSReset()
		{
			//TODO: Figure out how to reset the GPS chip
			FSWPacket * ret = new FSWPacket(0, GPS_RESET_FAILURE, false, true, MESSAGE_TYPE_ERROR);
			return ret;
		}

		bool BESTXYZProcess(char * buffer,const size_t size)
		{
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			char * parseBuffer;
			char * bufferPtr;
			double doubleHolder;

			int i = 0;
			int j = 0;
			while(buffer[i] != ';')
			{
				gpsData->header[j++] = buffer[i++];
				if(j >= 100)
				{
					printf("Header too long\n");
					return false;
				}
			}
			i++;
			j = 0;

			while(buffer[i] != '*')
			{
				gpsData->message[j++] = buffer[i++];
				if(j >= 250)
				{
					printf("Message too long\n");
					return false;
				}
			}

			i++;
			j = 0;
			while(buffer[i] != '\r')
			{
				gpsData->crc[j++] = buffer[i++];
				if(j >= 10)
				{
					printf("CRC too long\n");
					return false;
				}
			}

			// ------- HEADER ----------------------------------------------------------------------------------------------------------------------------------------------
			parseBuffer = gpsData->header;

			i = 0;
			j = 0;
			while(parseBuffer[i] != ',')
			{
				gpsData->MessageID[j++] = parseBuffer[i++];
				if(j >= 10)
				{
					return false;
				}
			}

			i++;
			j = 0;

			while(parseBuffer[i] != ',')
			{
				gpsData->Port[j++] = parseBuffer[i++];
				if(j >= 5)
				{
					return false;
				}
			}
			i++;

			gpsData->SequenceNum = strtol(&(parseBuffer[i]), (&bufferPtr), 10);
			if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			gpsData->IdleTime = strtod(parseBuffer, &bufferPtr);
			if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			i=0;
			j=0;
			while(parseBuffer[i] != ',')
			{
				gpsData->TimeStatus[j++] = parseBuffer[i++];
				if(j >= 15)
				{
					//debug_led_set_led(2, LED_TOGGLE);
					return false;
				}
			}
			i++;

			gpsData->GPSWeek = strtol(&(parseBuffer[i]), (&bufferPtr), 10);
			if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;


			gpsData->GPSSec = strtod(parseBuffer, (&bufferPtr));
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',') //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				return false;
			}

			printf("SequenceNum: %i\n", gpsData->SequenceNum);
			printf("GPSWeek:     %i\n", gpsData->GPSWeek);
			printf("GPSSec:      %f\n", gpsData->GPSSec);

			// ------- BESTXYZ ---------------------------------------------------------------------------------------------------------------------------------------------
			float params[5] = {0};
			parseBuffer = gpsData->message;
			i = 0;
			j = 0;

			while(parseBuffer[i] != ',')
			{
				gpsData->pSolStatus[j++] = parseBuffer[i++];
				if(j >= 15)
				{
					return false;
				}
			}
			i++;
			j = 0;

			while(parseBuffer[i] != ',')
			{
				gpsData->pSolType[j++] = parseBuffer[i++];
				if(j >= 15)
				{
					return false;
				}
			}
			i++;

			gpsData->posX = strtod(&(parseBuffer[i]), (&bufferPtr));
			if(bufferPtr[0] != ',')
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			for(j = 0; j < 5; j++)
			{
				doubleHolder = strtod(parseBuffer, &bufferPtr);
				if(bufferPtr[0] != ',')
				{
					return false;
				}

				parseBuffer = bufferPtr;
				parseBuffer++;
				params[j] = (float) doubleHolder;
			}

			//gpsData->posX = params[0];
			gpsData->posY = params[0];
			gpsData->posZ = params[1];
			gpsData->stdDevPX = params[2];
			gpsData->stdDevPY = params[3];
			gpsData->stdDevPZ = params[4];


			i = 0;
			j = 0;
			while(parseBuffer[i] != ',')
			{
				gpsData->vSolStatus[j++] = parseBuffer[i++];
				if(j >= 15)
				{
					return false;
				}
			}
			i++;
			j = 0;

			while(parseBuffer[i] != ',')
			{
				gpsData->vSolType[j++] = parseBuffer[i++];
				if(j >= 15)
				{
					return false;
				}
			}
			i++;

			gpsData->velX = strtod(&(parseBuffer[i]), (&bufferPtr));
			if(bufferPtr[0] != ',')
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			for(j = 0; j < 5; j++)
			{
				doubleHolder = strtod(parseBuffer, &bufferPtr);
				if(bufferPtr[0] != ',')
				{
					return false;
				}

				parseBuffer = bufferPtr;
				parseBuffer++;
				params[j] = (float) doubleHolder;
			}

			gpsData->velY = params[0];
			gpsData->velZ = params[1];
			gpsData->stdDevVX = params[2];
			gpsData->stdDevVY = params[3];
			gpsData->stdDevVZ = params[4];

			i = 0;
			j = 0;
			while(parseBuffer[i] != ',')
			{
				gpsData->stnID[j++] = parseBuffer[i++];
				if(j >= 10)
				{
					return false;
				}
			}
			i++;

			gpsData->latency = strtod(&(parseBuffer[i]), (&bufferPtr));
			if(bufferPtr[0] != ',')
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			gpsData->diffAge = strtod(parseBuffer, (&bufferPtr));
			if(bufferPtr[0] != ',')
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			gpsData->solAge = strtod(parseBuffer, (&bufferPtr));
			if(bufferPtr[0] != ',')
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;

			gpsData->numTracked = strtol(parseBuffer, (&bufferPtr), 10);
			if(bufferPtr[0] != ',')
			{
				return false;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			gpsData->numSolution = strtol(parseBuffer, (&bufferPtr), 10);

			return true;
		}

		bool GPRMCProcess(char * buffer, const size_t size){
			return false;
		}
	}
}
