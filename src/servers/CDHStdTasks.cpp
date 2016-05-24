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
#include "util/itoa.h"
#include "util/FileHandler.h"
#include "core/Singleton.h"

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
		// For Handlers -------------------------------------------------------------------
		ReturnMessage * CDHCPUUsage(void)
		{

			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(sysinfo(&cdhServer->si) != 0)
			{
				cout<<"CDHStdTasks: CDHCPUUsage(): Error"<<endl;
				ErrorMessage err(CDH_CPU_USAGE_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			VariableTypeData oneMinHold(cdhServer->si.loads[0]);
			VariableTypeData fiveMinHold(cdhServer->si.loads[1]);
			VariableTypeData fifteenMinHold(cdhServer->si.loads[2]);
			//VariableTypeData testHold(true);

			list<VariableTypeData *> params;
			params.push_back(&oneMinHold);
			params.push_back(&fiveMinHold);
			params.push_back(&fifteenMinHold);
			//params.push_back(&testHold);

			DataMessage * dat = new DataMessage(CDH_CPU_USAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(dat, true);
			delete dat;
			return retMsg;

		}

		ReturnMessage * CDHMemUsage(void)
		{
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(sysinfo(&cdhServer->si) != 0)
			{
				cout<<"CDHStdTasks: CDHMemUsage(): Error"<<endl;
				ErrorMessage err(CDH_MEM_USAGE_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			VariableTypeData memHold(100.0*(259964928.0 - ((float) cdhServer->si.freeram)) / (259964928.0)); //hard-coded total ram: 100*(total - free)/total = percent use

			list<VariableTypeData *> params;
			params.push_back(&memHold);

			DataMessage dat(CDH_MEM_USAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		//TODO: Fix for updated SD card mounting
		ReturnMessage * CDHStorage(void)
		{
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(statvfs((char *) "/", &cdhServer->svfs) != 0)
			{
				cout<<"CDHStdTasks: CDHStorage(): Error"<<endl;
				ErrorMessage err(CDH_STORAGE_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			cout<<"Free blocks: "<<cdhServer->svfs.f_bfree<<endl;
			cout<<"Total blocks: "<<cdhServer->svfs.f_blocks<<endl;

			VariableTypeData storageHold((uint32) cdhServer->svfs.f_bfree);

			list<VariableTypeData *> params;
			params.push_back(&storageHold);

			DataMessage dat(CDH_STORAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		ReturnMessage * CDHTempStart(void)
		{
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
				DataMessage msg(CDH_TEMP_START_SUCCESS);
				ReturnMessage * ret = new ReturnMessage(&msg, true);
				return ret;
			}else{
				ErrorMessage err(CDH_TEMP_START_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}
		}

		ReturnMessage * CDHTempRead(void)
		{
			// Setup
			float temperatures[4][16];
			list<VariableTypeData *> params;
			VariableTypeData tempHold[4][16];

			// Read and add to list
			for(uint8 bus = 0; bus < 4; bus++){
				for(uint8 sensor = 0; sensor < 16; sensor++){
					temperatures[bus][sensor] = ReadSensor(bus,sensor);
					tempHold[bus][sensor] = VariableTypeData(temperatures[bus][sensor]);
					params.push_back(&tempHold[bus][sensor]);
				}
			}

			// Send return
			DataMessage dat(CDH_TEMP_READ_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

//		ReturnMessage * CDHHotSwaps(void)
//		{
//			// Setup
//			float voltages[16];
//			float currents[16];
//			list<VariableTypeData *> params;
//			VariableTypeData voltageHold[16];
//			VariableTypeData currentHold[16];
//
//			// Read and add to list
//			for(uint8 i = 0; i < 16; i++){
//				CDHServer::hotSwaps[i]->Status(&voltages[i],&currents[i]);
//				voltageHold[i] = VariableTypeData(voltages[i]);
//				currentHold[i] = VariableTypeData(currents[i]);
//				params.push_back(&voltageHold[i]);
//				params.push_back(&currentHold[i]);
//			}
//
//			DataMessage dat(CDH_HOT_SWAPS_SUCCESS, params);
//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
//			return retMsg;
//		}

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
				cout<<"Error starting sensor"<<endl;
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
					cout<<"GOOD DATA"<<endl;
					delete c;
					delete tempRead;
					fclose(fp);
					return temperature;
				}else{
					cout<<"BAD DATA"<<endl;
					delete c;
					delete tempRead;
					fclose(fp);
					return -300;
				}
			}else{
				cout<<"Error opening file!"<<endl;
				return -301;
			}
		}
	}
}
