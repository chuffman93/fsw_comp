/*
 * CDHStdTasks.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"

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
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(sysinfo(&cdhServer->si) != 0){
				logger->Log("CDHStdTasks: CDHCPUUsage(): Error", LOGGER_LEVEL_ERROR);
				FSWPacket * ret = new FSWPacket(0, CDH_CPU_USAGE_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}

			logger->Log("CDHCPUUsage(): Checking loads", LOGGER_LEVEL_INFO);
			logger->Log("    CPU 1  min: %u", cdhServer->si.loads[0], LOGGER_LEVEL_DEBUG);
			logger->Log("    CPU 5  min: %u", cdhServer->si.loads[1], LOGGER_LEVEL_DEBUG);
			logger->Log("    CPU 15 min: %u", cdhServer->si.loads[2], LOGGER_LEVEL_DEBUG);

			uint8 * buffer = (uint8 *) malloc(12);
			AddUInt32(buffer, cdhServer->si.loads[0]);
			AddUInt32(buffer + 4, cdhServer->si.loads[1]);
			AddUInt32(buffer + 8, cdhServer->si.loads[2]);

			FSWPacket * ret = new FSWPacket(0, CDH_CPU_USAGE_SUCCESS, true, true, MESSAGE_TYPE_DATA, 12, buffer);
			return ret;
		}

		FSWPacket * CDHMemUsage(void)
		{
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(sysinfo(&cdhServer->si) != 0){
				logger->Log("CDHStdTasks: CDHMemUsage(): Error", LOGGER_LEVEL_ERROR);
				FSWPacket * ret = new FSWPacket(0, CDH_MEM_USAGE_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}

			logger->Log("CDHStdTasks: CDHMemUsage(): Checking Memory", LOGGER_LEVEL_INFO);
			float mem = 100.0*(259964928.0 - ((float) cdhServer->si.freeram)) / (259964928.0); //hard-coded total ram: 100*(total - free)/total = percent use
			logger->Log("    MEM: %f", mem, LOGGER_LEVEL_DEBUG);


			uint8 * buffer = (uint8 *) malloc(4);
			AddFloat(buffer, mem);

			FSWPacket * ret = new FSWPacket(0, CDH_MEM_USAGE_SUCCESS, true, true, MESSAGE_TYPE_DATA, 4, buffer);
			return ret;
		}

		//TODO: Fix for updated SD card mounting
		FSWPacket * CDHStorage(void)
		{
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(statvfs((char *) "/", &cdhServer->svfs) != 0){
				logger->Log("CDHStdTasks: CDHStorage(): Error", LOGGER_LEVEL_ERROR);
				FSWPacket * ret = new FSWPacket(0, CDH_STORAGE_FAILURE, false, true, MESSAGE_TYPE_ERROR);
				return ret;
			}

			logger->Log("CDHStdTasks: CDHStorage(): Checking storage", LOGGER_LEVEL_INFO);
			uint32 free = cdhServer->svfs.f_bfree;

			uint8 * buffer = (uint8 *) malloc(4);
			AddUInt32(buffer, free);

			FSWPacket * ret = new FSWPacket(0, CDH_STORAGE_SUCCESS, true, true, MESSAGE_TYPE_DATA, 4, buffer);
			return ret;
		}

		FSWPacket * CDHTempStart(void)
		{
			// Start all of the sensors
			bool validStart[4][16];
			bool success = true;
			for(uint8 bus = 0; bus < 4; bus++){
				for(uint8 sensor = 0; sensor < 16; sensor++){
					validStart[bus][sensor] = StartTempSensor(bus+1,sensor);
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
			logger->Log("CDHStdTasks: CDHTempRead(): Reading temp sensors", LOGGER_LEVEL_INFO);

			// Setup
			float temperatures[4][16];
			uint8 * buffer = (uint8 *) malloc(64*sizeof(float));

			// Read and add to list
			for(uint8 bus = 0; bus < 4; bus++){
				for(uint8 sensor = 0; sensor < 16; sensor++){
					temperatures[bus][sensor] = ReadTempSensor(bus+1,sensor);
					AddFloat(buffer + 4*(16*bus + sensor), temperatures[bus][sensor]);
				}
			}

			// Send return
			FSWPacket * ret = new FSWPacket(0, CDH_TEMP_READ_SUCCESS, true, true, MESSAGE_TYPE_DATA, 64*sizeof(float), buffer);
			return ret;
		}

		FSWPacket * CDHHotSwaps(void)
		{
			logger->Log("CDHStdTasks: CDHHotSwaps(): Reading hot swaps", LOGGER_LEVEL_INFO);

			// Setup
			Phoenix::Servers::CDHServer * cdhServer = dynamic_cast<Phoenix::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
			float vcRead[32];
			uint8 * buffer = (uint8 *) malloc(32*sizeof(float));

			// Read and add to buffer
			cdhServer->devMan->getHSStatus(vcRead);
			for(uint8 i = 0; i < 32; i++){
				AddFloat(buffer + i, vcRead[i]);
			}

			FSWPacket * ret = new FSWPacket(0, CDH_HOT_SWAPS_SUCCESS, true, true, MESSAGE_TYPE_DATA, 64*sizeof(float), buffer);
			return ret;
		}

		//TODO: add back power monitors
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
		bool StartTempSensor(int bus, int sensor)
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
				logger->Log("CDHStdTasks: StartTempSensor(): Error Starting Sensor!", LOGGER_LEVEL_WARN);
				return false;
			}
			return true;
		}

		float ReadTempSensor(int bus, int sensor){

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
			char logbuf[150];

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
					sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Good data from sensor %d on bus %d",sensor,bus);
					logger->Log(logbuf,LOGGER_LEVEL_DEBUG);
					delete c;
					delete tempRead;
					fclose(fp);
					return temperature;
				}else{
					sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Bad data from sensor %d on bus %d!",sensor,bus);
					logger->Log(logbuf,LOGGER_LEVEL_WARN);
					delete c;
					delete tempRead;
					fclose(fp);
					return -300;
				}
			}else{
				sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Error opening file: sensor %d on bus %d",sensor,bus);
				logger->Log(logbuf,LOGGER_LEVEL_WARN);
				return -301;
			}
		}
	}
}
