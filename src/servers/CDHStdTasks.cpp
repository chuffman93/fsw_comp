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
#include <iostream>

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
			sysinfo(&cdhServer->si);
			cout<<"1 min: "<<cdhServer->si.loads[0]<<" <--------------------------------"<<endl;
			cout<<"5 min: "<<cdhServer->si.loads[1]<<" <--------------------------------"<<endl;
			cout<<"15 min: "<<cdhServer->si.loads[2]<<" <--------------------------------"<<endl;

			VariableTypeData oneMinHold(cdhServer->si.loads[0]);
			VariableTypeData fiveMinHold(cdhServer->si.loads[1]);
			VariableTypeData fifteenMinHold(cdhServer->si.loads[2]);

			/*
			VariableTypeData oneMinHold((float) 0);
			VariableTypeData fiveMinHold((float) 1);
			VariableTypeData fifteenMinHold((float) 2);
			*/

			list<VariableTypeData *> params;
			params.push_back(&oneMinHold);
			params.push_back(&fiveMinHold);
			params.push_back(&fifteenMinHold);

			DataMessage dat(CDH_CPU_USAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			cout<<"Handler success: "<<retMsg->GetSuccess()<<endl;
			cout<<"Handler opcode:  "<<retMsg->GetOpcode()<<endl;
			return retMsg;

		}

		ReturnMessage * CDHMemUsage(void)
		{
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			if(sysinfo(&cdhServer->si) != 0)
			{
				//add error
			}

			VariableTypeData memHold(100.0*(259964928.0 - ((float) cdhServer->si.freeram)) / (259964928.0)); //hard-coded total ram: 100*(total-free)/total = percent use

			list<VariableTypeData *> params;
			params.push_back(&memHold);

			DataMessage dat(CDH_MEM_USAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		ReturnMessage * CDHStorage(void)
		{

			//Actual usage code goes here------------------------------
			float storage = 11.0;
			//---------------------------------------------------------

			VariableTypeData storageHold(storage);

			list<VariableTypeData *> params;
			params.push_back(&storageHold);

			DataMessage dat(CDH_STORAGE_SUCCESS, params);
			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
			return retMsg;
		}

		ReturnMessage * CDHTempBus(void)
		{

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
