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
		
		ReturnMessage * GPSHealthStatus()
		{
			
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				ErrorMessage err(GPS_HS_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				ErrorMessage err(GPS_HS_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			
			uint8 buff[350];
			memcpy(buff, gpsData->header, 100);
			memcpy((buff + 100), gpsData->message, 250);
			DataMessage dat(GPS_HS_SUCCESS, VariableTypeData(buff, 350));
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			
			//COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
			//FIXME: figure out why eclipse dosent like this lines
//			comServer -> UpdateGPSHS(GPSData->GPSWeek,GPSData->GPSSec,GPSData->posX,GPSData->posY,GPSData->posZ,
//				GPSData->velX,GPSData->velY,GPSData->velZ,GPSData->numSolution,GPSData->solAge);
				
			return retMsg;
		}

		ReturnMessage * GPSTime()
		{
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				ErrorMessage err(GPS_TIME_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				ErrorMessage err(GPS_TIME_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			
			int32 week = gpsData->GPSWeek;
			float sec = gpsData->GPSSec;
			VariableTypeData weekHold(week);
			VariableTypeData secHold(sec);
			
			list<VariableTypeData *> params;
			params.push_back(&weekHold);
			params.push_back(&secHold);
			
			DataMessage dat(GPS_TIME_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		ReturnMessage * GPSPostion()
		{
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				ErrorMessage err(GPS_POSITION_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				ErrorMessage err(GPS_POSITION_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			
			VariableTypeData x_hold(gpsData->posX);
			VariableTypeData y_hold(gpsData->posY);
			VariableTypeData z_hold(gpsData->posZ);

			list<VariableTypeData *> params;
			params.push_back(&x_hold);
			params.push_back(&y_hold);
			params.push_back(&z_hold);

			DataMessage dat(GPS_POSITION_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		//Talk to ACS to convert ECEF -> ECI
		bool GPSConversion()
		{
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			if (NULL == gpsServer)
			{
				ErrorMessage err(GPS_TIME_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				ErrorMessage err(GPS_POSITION_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				return eRet;
			}

			VariableTypeData posX_hold(gpsData->posX);
			VariableTypeData posY_hold(gpsData->posY);
			VariableTypeData posZ_hold(gpsData->posZ);
			VariableTypeData velX_hold(gpsData->velX);
			VariableTypeData velY_hold(gpsData->velY);
			VariableTypeData velZ_hold(gpsData->velZ);
			VariableTypeData sec_hold(gpsData->GPSSec);
			VariableTypeData weekHold((uint32) gpsData->GPSWeek);

			list<VariableTypeData *> params;
			params.push_back(&posX_hold);
			params.push_back(&posY_hold);
			params.push_back(&posZ_hold);
			params.push_back(&velX_hold);
			params.push_back(&velY_hold);
			params.push_back(&velZ_hold);
			params.push_back(&sec_hold);

			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_GPS, SERVER_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_GPS_CMD, params);
			if(!ret->GetSuccess())
			{
				return false;
			}
			
			void * outputArray[6] = {NULL};
			if(!ExtractParameters((*ret), GPSConversionEnumArray, 6, outputArray))
			{
				return false;
			}

			gpsData->posECIX= * (float *) outputArray[0];
			gpsData->posECIY= * (float *) outputArray[1];
			gpsData->posECIZ= * (float *) outputArray[2];
			gpsData->velECIX= * (float *) outputArray[3];
			gpsData->velECIY= * (float *) outputArray[4];
			gpsData->velECIZ= * (float *) outputArray[5];
			
			return true;
		}

		ReturnMessage * GPSReset()
		{
			//TODO: Figure out how to reset the GPS chip
			ErrorMessage err(GPS_RESET_FAILURE);
			ReturnMessage * retMsg = new ReturnMessage(&err, false);
			return retMsg;
		}

		void GPSDataProcess(char * buffer,const size_t size)
		{
//			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
			//volatile int indexCounter = 0;
			
			GPSData * gpsData = gpsServer->GetGPSDataPtr();
			if(gpsData == NULL)
			{
				//debug_led_set_led(6, LED_TOGGLE);
				return;
			}

			//Remove ths stuff before the buffer
			size_t bufferStart;
			int buffCounter=0;
			for(bufferStart=0;bufferStart<size;bufferStart++){
				if(buffer[bufferStart]=='\n'){
					buffCounter++;
					if(buffCounter==2){
						break;
					}
				}
			}
			if(bufferStart >= (size - 7))
			{
				//debug_led_set_led(5, LED_TOGGLE);
				return;
			}//Error

			bufferStart+=7;
			buffer += bufferStart+1;
			//indexCounter += bufferStart+1;
			
			char * parseBuffer;
			char * bufferPtr;
			double doubleHolder;
			//Split into Header, Message, and CRC according to ascii message definition on pg 20 of 
			//  
			//sscanf( buffer,"[^#]#%[^;];%[^*]*%s",GPS.header,GPS.message,GPS.crc);
			//sscanf( buffer,"%s;%[^*]*%s",GPS.header,GPS.message,GPS.crc);
			//sscanf( buffer,"#%[^;];%[^\\*]\\*%s", gpsData->header ,gpsData->message ,gpsData->crc);
			
			
			int i = 0;
			int j = 0;
			while(buffer[i] != ';' /*&& j < 100*/)
			{
				gpsData->header[j++] = buffer[i++];
				//indexCounter++;
				if(j >= 100)
				{
					//debug_led_set_led(1, LED_TOGGLE);
					return;
				}
			}
			i++;
			j = 0;
			
			while(buffer[i] != '*' /*&& j < 250*/)
			{
				gpsData->message[j++] = buffer[i++];
				//indexCounter++;
				if(j >= 250)
				{
					//debug_led_set_led(2, LED_TOGGLE);
					return;
				}
			}
			
			i++;
			j = 0;
			while(buffer[i] != '\r' /*&& j < 10*/)
			{
				gpsData->crc[j++] = buffer[i++];
				//indexCounter++;
				if(j >= 10)
				{
					//debug_led_set_led(3, LED_TOGGLE);
					return;
				}
			}
			
			
			//Parse Header according to ASCII Header Definition on pg 21 of this document 
			// http://www.novatel.com/assets/Documents/Manuals/om-20000094.pdf
			//sscanf(gpsData->header,"%[^,],%[^,],%d,%f,%[^,],%d,%f", gpsData->MessageID, gpsData->Port, &gpsData->SequenceNum, &gpsData->IdleTime,
			//gpsData->TimeStatus, &gpsData->GPSWeek, &gpsData->GPSSec);
			parseBuffer = gpsData->header;
			
			i = 0;
			j = 0;
			while(parseBuffer[i] != ',' /*&& j <  10*/)
			{
				gpsData->MessageID[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 10)
				{
					//debug_led_set_led(4, LED_TOGGLE);
					return;
				}
			}
			
			i++;
			j = 0;
			
			while(parseBuffer[i] != ',' /*&& j < 5*/)
			{
				gpsData->Port[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 5)
				{
					//debug_led_set_led(5, LED_TOGGLE);
					return;
				}
			}
			i++;
			
			gpsData->SequenceNum = strtol(&(parseBuffer[i]), (&bufferPtr), 10);
			//indexCounter += &(parseBuffer[i]) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//error stuff
				//debug_led_set_led(6, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			gpsData->IdleTime = strtod(parseBuffer, &bufferPtr);
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//error stuff
				//debug_led_set_led(1, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			i=0;
			j=0;
			while(parseBuffer[i] != ',' /*&& j < 15*/)
			{
				gpsData->TimeStatus[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 15)
				{
					//debug_led_set_led(2, LED_TOGGLE);
					return;
				}
			}
			i++;
			
			gpsData->GPSWeek = strtol(&(parseBuffer[i]), (&bufferPtr), 10);
			//indexCounter += &(parseBuffer[i]) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//debug_led_set_led(3, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			

			gpsData->GPSSec = strtod(parseBuffer, (&bufferPtr));
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//debug_led_set_led(4, LED_TOGGLE);
				return;
			}
		
			
			
			//Parse BESTXYZ Message up to Number of Satellites used in solution (param 23) according to pg 263 of 
			// http://www.novatel.com/assets/Documents/Manuals/om-20000094.pdf
			//sscanf(gpsData->message,"%[^,],%[^,],%f,%f,%f,%f,%f,%f,%[^,],%[^,],%f,%f,%f,%f,%f,%f,%[^,],%f,%f,%f,%d,%d", gpsData->pSolStatus,
			//gpsData->pSolType, &gpsData->posX, &gpsData->posY, &gpsData->posZ, &gpsData->stdDevPX, &gpsData->stdDevPY, &gpsData->stdDevPZ, 
			//gpsData->vSolStatus, gpsData->vSolType, &gpsData->velX, &gpsData->velY, &gpsData->velZ, &gpsData->stdDevVX, &gpsData->stdDevVY, 
			//&gpsData->stdDevVZ, gpsData->stnID, &gpsData->latency, &gpsData->diffAge, &gpsData->solAge,&gpsData->numTracked,
			//&gpsData->numSolution);
			float params[5] = {0};
			parseBuffer = gpsData->message;
			i = 0;
			j = 0;
			
			while(parseBuffer[i] != ',' /*&& j < 15*/)
			{
				gpsData->pSolStatus[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 15)
				{
					//debug_led_set_led(5, LED_TOGGLE);
					return;
				}
			}
			i++;
			j = 0;
			
			while(parseBuffer[i] != ',' /*&& j < 15*/)
			{
				gpsData->pSolType[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 15)
				{
					//debug_led_set_led(6, LED_TOGGLE);
					return;
				}
			}
			i++;
			
			gpsData->posX = strtod(&(parseBuffer[i]), (&bufferPtr));
			//indexCounter += &(parseBuffer[i]) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
 				//debug_led_set_led(1, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			for(j = 0; j < 5; j++)
			{
				doubleHolder = strtod(parseBuffer, &bufferPtr);
				//indexCounter += (parseBuffer) - (bufferPtr);
				//if we are not extracting the 4th parameter
				if((bufferPtr[0] != ',' /*|| indexCounter >= size*/)) //and we don't get a comma next the last value was not extracted correctly, return error.
				{
 					//debug_led_set_led(2, LED_TOGGLE);
					return;
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
			while(parseBuffer[i] != ',' /*&& j < 15*/)
			{
				gpsData->vSolStatus[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 15)
				{
 					//debug_led_set_led(3, LED_TOGGLE);
					return;
				}
			}
			i++;
			j = 0;
			
			while(parseBuffer[i] != ',' /*&& j < 15*/)
			{
				gpsData->vSolType[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 15)
				{
 					//debug_led_set_led(4, LED_TOGGLE);
					return;
				}
			}
			i++;
			
			gpsData->velX = strtod(&(parseBuffer[i]), (&bufferPtr));
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
 				//debug_led_set_led(5, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			for(j = 0; j < 5; j++)
			{
				doubleHolder = strtod(parseBuffer, &bufferPtr);
				//indexCounter += (parseBuffer) - (bufferPtr);
				//if we are not extracting the 4th parameter
				if(bufferPtr[0] != ','  /*|| indexCounter >= size*/) //and we don't get a comma next the last value was not extracted correctly, return error.
				{
 					//debug_led_set_led(6, LED_TOGGLE);
					return;
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
			while(parseBuffer[i] != ',' /*&& j < 10*/)
			{
				gpsData->stnID[j++] = parseBuffer[i++];
				//indexCounter++;
				if(j >= 10)
				{
					//debug_led_set_led(1, LED_TOGGLE);
					return;
				}
			}
			i++;
			
			gpsData->latency = strtod(&(parseBuffer[i]), (&bufferPtr));
			//indexCounter += &(parseBuffer[i]) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//debug_led_set_led(2, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			gpsData->diffAge = strtod(parseBuffer, (&bufferPtr));
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//debug_led_set_led(3, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			gpsData->solAge = strtod(parseBuffer, (&bufferPtr));
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//debug_led_set_led(4, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			gpsData->numTracked = strtol(parseBuffer, (&bufferPtr), 10);
			//indexCounter += (parseBuffer) - (bufferPtr);
			if(bufferPtr[0] != ',' /*|| indexCounter >= size*/) //if we don't get a comma next the last value was not extracted correctly, return error.
			{
				//debug_led_set_led(5, LED_TOGGLE);
				return;
			}
			parseBuffer = bufferPtr;
			parseBuffer++;
			
			gpsData->numSolution = strtol(parseBuffer, (&bufferPtr), 10);
			
			//debug_led_set_led(7, LED_TOGGLE);
			//indexCounter += (parseBuffer) - (bufferPtr);
// 			if(indexCounter >= size) //if we don't get a comma next the last value was not extracted correctly, return error.
// 			{
// 				debug_led_set_led(6, LED_TOGGLE);
// 				return;
// 			}
					
			//GPSConversion();

			//parse return message and handle any erros.
			
			/*
			std::vector<uint8> data;
			data.assign(buffer, buffer + sizeof(buffer));
			
			VariableTypeData dataBuffer(data);
			CommandMessage dat(GPS_HS_CMD, dataBuffer);

			// check bounds here!
			if(!fileHandler->Append(SUBSYSTEM_GPS, GPS_HS_CMD, dat))
			{
				// write to error log
			}
			*/
			//TODO:RTCTHIGNS
			//RTCSetTime((uint32) gpsData->GPSSec);

			/*
			if (GPS_BOUNDS_LOW[0][1] == retmsg)
			{


			}
			*/
		}
	}
}
