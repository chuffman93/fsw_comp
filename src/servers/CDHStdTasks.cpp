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
#include "core/Singleton.h"

#include "util/itoa.h"
#include "util/FileHandler.h"
#include "util/Logger.h"

#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string.h>

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

		//------------------------------------------- Message Handlers -------------------------------------------

		FSWPacket * CDHCPUUsage(void)
		{
			logger->Log("CDHStdTasks: CDHCPUUsage(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

//			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
//			if(sysinfo(&cdhServer->si) != 0)
//			{
//				logger->Log("CDHStdTasks: CDHCPUUsage(): Error", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(CDH_CPU_USAGE_FAILURE);
//				ReturnMessage * ret = new ReturnMessage(&err, false);
//				return ret;
//			}
//
//			logger->Log("CDHStdTasks: CDHCPUUsage(): Checking loads", LOGGER_LEVEL_INFO);
//
//			VariableTypeData oneMinHold(cdhServer->si.loads[0]);
//			VariableTypeData fiveMinHold(cdhServer->si.loads[1]);
//			VariableTypeData fifteenMinHold(cdhServer->si.loads[2]);
//			//VariableTypeData testHold(true);
//
//			list<VariableTypeData *> params;
//			params.push_back(&oneMinHold);
//			params.push_back(&fiveMinHold);
//			params.push_back(&fifteenMinHold);
//			//params.push_back(&testHold);
//
//			DataMessage * dat = new DataMessage(CDH_CPU_USAGE_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(dat, true);
//			delete dat;
//			return retMsg;

		}

		FSWPacket * CDHMemUsage(void)
		{
			logger->Log("CDHStdTasks: CDHMemUsage(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

//			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
//			if(sysinfo(&cdhServer->si) != 0)
//			{
//				logger->Log("CDHStdTasks: CDHMemUsage(): Error", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(CDH_MEM_USAGE_FAILURE);
//				ReturnMessage * ret = new ReturnMessage(&err, false);
//				return ret;
//			}
//
//			logger->Log("CDHStdTasks: CDHMemUsage(): Checking Memory", LOGGER_LEVEL_INFO);
//			VariableTypeData memHold(100.0*(259964928.0 - ((float) cdhServer->si.freeram)) / (259964928.0)); //hard-coded total ram: 100*(total - free)/total = percent use
//
//			list<VariableTypeData *> params;
//			params.push_back(&memHold);
//
//			DataMessage dat(CDH_MEM_USAGE_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		//TODO: Fix for updated SD card mounting
		FSWPacket * CDHStorage(void)
		{
			logger->Log("CDHStdTasks: CDHStorage(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

//			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
//			if(statvfs((char *) "/", &cdhServer->svfs) != 0)
//			{
//				logger->Log("CDHStdTasks: CDHStorage(): Error", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(CDH_STORAGE_FAILURE);
//				ReturnMessage * ret = new ReturnMessage(&err, false);
//				return ret;
//			}
//
//			logger->Log("CDHStdTasks: CDHStorage(): Checking storage", LOGGER_LEVEL_INFO);
//
//			VariableTypeData storageHold((uint32) cdhServer->svfs.f_bfree);
//
//			list<VariableTypeData *> params;
//			params.push_back(&storageHold);
//
//			DataMessage dat(CDH_STORAGE_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		FSWPacket * CDHTempStart(void)
		{
			logger->Log("CDHStdTasks: CDHTempStart(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

			// Start all of the sensors
			bool validStart[4][16];
			bool success = true;
			for(uint8 bus = 1; bus < 5; bus++){
				for(uint8 sensor = 0; sensor < 16; sensor++){
					validStart[bus][sensor] = StartSensor(bus,sensor);
					success &= validStart[bus][sensor];
				}
			}

			if(success){
				logger->Log("CDHStdTasks: CDHTempStart(): Started sensors", LOGGER_LEVEL_INFO);
				FSWPacket * ret = new FSWPacket(0, CDH_TEMP_START_SUCCESS, true, true, MESSAGE_TYPE_DATA);
				return ret;
			}else{
				logger->Log("CDHStdTasks: CDHTempStart(): Error starting sensors!", LOGGER_LEVEL_ERROR);
				FSWPacket * ret = new FSWPacket(0, CDH_TEMP_START_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}
		}

		FSWPacket * CDHTempRead(void)
		{
			logger->Log("CDHStdTasks: CDHTempRead(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

//			// Setup
//			float temperatures[4][16];
//			list<VariableTypeData *> params;
//			VariableTypeData tempHold[4][16];
//
//			// Read and add to list
//			for(uint8 bus = 0; bus < 4; bus++){
//				for(uint8 sensor = 0; sensor < 16; sensor++){
//					temperatures[bus][sensor] = ReadSensor(bus,sensor);
//					tempHold[bus][sensor] = VariableTypeData(temperatures[bus][sensor]);
//					params.push_back(&tempHold[bus][sensor]);
//				}
//			}
//
//			// Send return
//			logger->Log("CDHStdTasks: CDHTempRead(): Read sensors", LOGGER_LEVEL_INFO);
//			DataMessage dat(CDH_TEMP_READ_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		FSWPacket * CDHHotSwaps(void)
		{
			logger->Log("CDHStdTasks: CDHHotSwaps(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

//			// Setup
//			Phoenix::Servers::CDHServer * cdhServer = dynamic_cast<Phoenix::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
//			float vcRead[32];
//			list<VariableTypeData *> params;
//			VariableTypeData voltageHold[16];
//			VariableTypeData currentHold[16];
//
//			// Read and add to list
//			cdhServer->devMan->getHSStatus(vcRead);
//			for(uint8 i = 0; i < 16; i++){
//				voltageHold[i] = VariableTypeData(vcRead[i]);
//				currentHold[i] = VariableTypeData(vcRead[i+16]);
//				params.push_back(&voltageHold[i]);
//				params.push_back(&currentHold[i]);
//			}
//
//			logger->Log("CDHStdTasks: CDHHotSwaps(): Read Hot Swaps", LOGGER_LEVEL_INFO);
//			DataMessage dat(CDH_HOT_SWAPS_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		//TODO: check more information
		FSWPacket * CDHPowerMonitors(void)
		{
			logger->Log("CDHStdTasks: CDHPowerMonitors(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

//			Phoenix::Servers::CDHServer * cdhServer = dynamic_cast<Phoenix::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
//			PowerMonitor_Data data[4];
//			list<VariableTypeData *>params;
//			VariableTypeData maxPHold[4];
//			VariableTypeData minPHold[4];
//			VariableTypeData maxVHold[4];
//			VariableTypeData minVHold[4];
//			VariableTypeData maxAHold[4];
//			VariableTypeData minAHold[4];
//			VariableTypeData maxIHold[4];
//			VariableTypeData minIHold[4];
//
//			// Read and add to list
//			cdhServer->devMan->getPMStatus(data);
//			for(uint8 i = 0; i < 4; i++){
//				maxPHold[i] = VariableTypeData(data[i].MaxPower);
//				minPHold[i] = VariableTypeData(data[i].MinPower);
//				maxVHold[i] = VariableTypeData(data[i].MaxVoltage);
//				minVHold[i] = VariableTypeData(data[i].MinVoltage);
//				maxAHold[i] = VariableTypeData(data[i].MaxADIN);
//				minAHold[i] = VariableTypeData(data[i].MinADIN);
//				maxIHold[i] = VariableTypeData(data[i].MaxCurrent);
//				minIHold[i] = VariableTypeData(data[i].MinCurrent);
//				params.push_back(&maxPHold[i]);
//				params.push_back(&minPHold[i]);
//				params.push_back(&maxVHold[i]);
//				params.push_back(&minVHold[i]);
//				params.push_back(&maxAHold[i]);
//				params.push_back(&minAHold[i]);
//				params.push_back(&maxIHold[i]);
//				params.push_back(&minIHold[i]);
//			}
//
//			DataMessage dat(CDH_POWER_MONITORS_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
		}

		FSWPacket * CDHStartPM(void)
		{
			logger->Log("CDHStdTasks: CDHStartPM(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

			Phoenix::Servers::CDHServer * cdhServer = dynamic_cast<Phoenix::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));

			// Start all of the sensors
			cdhServer->devMan->startPMMeas();

			logger->Log("CDHStdTasks: CDHStartPM(): Started PM measurement", LOGGER_LEVEL_INFO);
			FSWPacket * ret = new FSWPacket(0, CDH_START_PM_SUCCESS, true, true, MESSAGE_TYPE_DATA);
			return ret;
		}

		// Helper Functions ---------------------------------------------------------------
		bool StartSensor(int bus, int sensor)
		{
			// create filename
			char * temp = new char[1];
			string start = "echo 1 > /sys/bus/w1/devices/w1_bus_master";
			itoa(bus, temp, 10);
			start.append(temp);
			start.append("/TEMP");
			itoa(sensor, temp, 10);
			start.append(temp);
			start.append("/start");
			delete temp;

			// start sensor
			if(system(start.c_str()) == -1){
				logger->Log("CDHStdTasks: StartSensor(): Error Starting Sensor!", LOGGER_LEVEL_WARN);
				return false;
			}
			return true;
		}

		float ReadSensor(int bus, int sensor){

			// create filename
			char * temp = new char[1];
			string read = "/sys/bus/w1/devices/w1_bus_master";
			itoa(bus, temp, 10);
			read.append(temp);
			read.append("/TEMP");
			itoa(sensor, temp, 10);
			read.append(temp);
			read.append("/temp");
			delete temp;
			char logbuf[80];

			FILE * fp;
			fp = fopen(read.c_str(), "r");

			//cout<<"Attempting to read sensor "<<sensor<<" on bus "<<bus<<"!"<<endl;

			bool isGood = false;
			if(fp)
			{
				char * c = new char[1];
				char * tempRead = new char[9];
				int tempHold;
				int count = 0;
				float temperature;

				// Get temperature part of string
				while((*c = fgetc(fp)) != '\n')
				{
					tempRead[count] = *c;
					count++;
				}

				// Get float value
				sscanf(tempRead, "t=%d", &tempHold);
				temperature = (float) tempHold / 1000.0;
				//cout<<"Current Temperature: "<<temperature<<endl;

				// Check validity
				for(int i = 0; i < 28; i++){
					*c = fgetc(fp);
					if(i==27){
						if(*c=='V'){
							isGood = true;
						}
					}
				}

				// Act on validity
				if(isGood){
					sprintf(logbuf,"CDHStdTasks: ReadSensor: Good data from sensor %d on bus %d",sensor,bus);
					logger->Log(logbuf,LOGGER_LEVEL_DEBUG);
					delete c;
					delete tempRead;
					fclose(fp);
					return temperature;
				}else{
					sprintf(logbuf,"CDHStdTasks: ReadSensor: Bad data from sensor %d on bus %d!",sensor,bus);
					logger->Log(logbuf,LOGGER_LEVEL_WARN);
					delete c;
					delete tempRead;
					fclose(fp);
					return -300;
				}
			}else{
				sprintf(logbuf,"CDHStdTasks: ReadSensor: Error opening file: sensor %d on bus %d",sensor,bus);
				logger->Log(logbuf,LOGGER_LEVEL_ERROR);
				return -301;
			}
		}
	}
}
