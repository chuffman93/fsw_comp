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

#include "util/FileHandler.h"

#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string.h>

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
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

			list<VariableTypeData *> params;
			params.push_back(&oneMinHold);
			params.push_back(&fiveMinHold);
			params.push_back(&fifteenMinHold);

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
			if(statvfs((char *) "/media/sdMount/", &cdhServer->svfs) != 0)
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

		//TODO: Determine file read specification
		/*
		 * - Standard char number?
		 * - Number of files?
		 * - If sensor goes down (ie. file DNE)
		*/
		ReturnMessage * CDHTempBus(void)
		{
			// Eventually make string arrays for iteration
			char * cdh1s = (char *) "echo 1 >> /sys/bus/w1/devices/w1_bus_master1/TEMP0/start";
			char * cdh1t = (char *) "/sys/bus/w1/devices/w1_bus_master1/TEMP0/temp";
			//string cdh2 = "/sys/bus/w1/devices/w1_bus_master1/TEMP1/";

			if(system(cdh1s) == -1){
				//error
				cout<<"CDHStdTasks CHDTempBus(): Error starting sensor"<<endl;
			}
			usleep(600000);

			FILE * fp;

			fp = fopen(cdh1t, "r");

			cout<<"CDHStdTasks CHDTempBus(): attempting to read sensor!"<<endl;

			if(fp)
			{
				uint8 * buffer = NULL;
				uint8 bufferSize = 128;
				size_t result;
				buffer = new uint8[bufferSize];
				result = fread(buffer, 1, bufferSize, fp);

				for (int i = 0; i < 128; i++) {
					cout << buffer[i];
				}

				delete buffer;
			}
			else
			{
				cout<<"CDHStdTasks CHDTempBus(): Error opening file"<<endl;
			}


			//Actual temperature code goes here------------------------------
			float tempbus;
			tempbus = 22.3;
			//---------------------------------------------------------

			VariableTypeData tempbusHold(tempbus);

			list<VariableTypeData *> params;
			params.push_back(&tempbusHold);

			DataMessage dat(CDH_TEMP_BUS_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		//TODO: Finish HotSwaps (meet with CDH)
		ReturnMessage * CDHHotSwaps(void)
		{

			//Actual hot swap code goes here------------------------------
			float hotswaps;
			hotswaps = 22.8;
			//---------------------------------------------------------

			VariableTypeData hotswapHold(hotswaps);

			list<VariableTypeData *> params;
			params.push_back(&hotswapHold);

			DataMessage dat(CDH_HOT_SWAPS_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}
	}
}
