///*
// * THMStdTasks.cpp
// * Written by: Caitlyn
// * Created: 12/1/12
// */
//
//#include <list>
//
//#include "servers/THMStdTasks.h"
//#include "servers/DispatchStdTasks.h"
//#include "servers/THMServer.h"
//
//#include "core/Dispatcher.h"
//
//#include "core/VariableTypeData.h"
//#include "core/DataMessage.h"
//
//
//
////#include "boards/backplane/dbg_led.h"
////#include "boards/board.h"
//
//using namespace std;
//using namespace AllStar::Core;
//
//
//namespace AllStar
//{
//	namespace Servers
//	{
//		ReturnMessage * DataCollect(uint32 busNumber)
//		{
//
//
///*
//			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
//			THMServer * thmServer = dynamic_cast<THMServer *>(Factory::GetInstance(THM_SERVER_SINGLETON));
//			uint16 * ubound = thmServer->getUpper();
//			uint16 * lbound = thmServer->getLower();
//
//			// array to hold the temperature values
//			uint16 values[16];
//			size_t index = 0;
//
//			list<VariableTypeData *> params;
//			VariableTypeData thmHolder;
//
//			uint8 busLow = 0;
//			uint8 busHigh = 0;
//
//			// Iterate through appropriate sensors based on desired bus
//			switch(busNumber)
//			{
//			case 0:
//				busLow = 0;
//				busHigh = 15;
//				break;
//			case 1:
//				busLow = 16;
//				busHigh = 31;
//				break;
//			case 2:
//				busLow = 32;
//				busHigh = 47;
//				break;
//			case 3:
//				busLow = 48;
//				busHigh = 63;
//				break;
//			}
//#ifdef HARDWARE
//			if (!TemperatureConvert())
//			{
//
//				//couldn't convert values for reading
//				//perhaps write an error and return
//
//			}
//
//			for (uint8 sensorNum = busLow; sensorNum <= busHigh; sensorNum++)
//			{
//				if (!TemperatureRead(sensorNum, (&values[index])))
//				{
//					//write to error log
//				}
//				else
//				{
//					values[index] = values[index] >> 4;
//				}
//				thmHolder = VariableTypeData((uint32) values[index]);
//				params.push_back(&thmHolder);
//				index++;
//			}
//
//			CommandMessage dat(THM_HS_CMD, params);
//
//			if(!fileHandler->Append(SUBSYSTEM_THM, THM_HS_CMD, dat))
//			{
//				//write to error log
//			}
//
//			for(size_t i = 0; i <= busHigh; i++)
//			{
//				uint32 sensor = i + busLow;
//				if(values[i] <= lbound[i+busLow])
//				{
//						list<VariableTypeData *> params;
//						VariableTypeData sensor_hold((uint32) sensor);
//						VariableTypeData temp_hold((uint32) values[i]);
//						VariableTypeData cold_hold((bool) THM_COLD);
//						params.push_back(&sensor_hold);
//						params.push_back(&temp_hold);
//						params.push_back(&cold_hold);
//						DispatchPacket(SERVER_LOCATION_THM, SERVER_LOCATION_ERR, 0, 0, MESSAGE_TYPE_ERROR, THM_HS_FAILURE, &params);
//						ErrorMessage err(THM_HS_FAILURE);
//						ReturnMessage * ret = new ReturnMessage(&err, true);
//						return ret;
//				}
//				if(values[i] >= ubound[i+busLow])
//				{
//						list<VariableTypeData *> params;
//						VariableTypeData sensor_hold((uint32) sensor);
//						VariableTypeData temp_hold((uint32) values[i]);
//						VariableTypeData hot_hold((bool) THM_HOT);
//						params.push_back(&sensor_hold);
//						params.push_back(&temp_hold);
//						params.push_back(&hot_hold);
//						DispatchPacket(SERVER_LOCATION_THM, SERVER_LOCATION_ERR, 0, 0, MESSAGE_TYPE_ERROR, THM_HS_FAILURE, &params);
//						ErrorMessage err(THM_HS_FAILURE);
//						ReturnMessage * ret = new ReturnMessage(&err, true);
//						return ret;
//				}
//			}
//#endif //HARDWARE
//*/
//
//			DataMessage msg(THM_HS_SUCCESS);
//			ReturnMessage * ret = new ReturnMessage(&msg, true);
//			return ret;
//		}
//
//		void IsDark(void)
//		{
//			//check BUS2:ALL, BUS3:1, 2, 3, 7, 8, 9
//			//if all are cold it is dark
//
//			// array to hold the temperature values
//			uint16 values[THERMAL_SENSORS_EXO_CHECK];
//			uint16 busLow = 16;
//			uint16 busHigh = 34;
//			uint8 numLow = 0;
//			uint32 time = 30000;
//#ifdef HARDWARE
//			if (!TemperatureConvert())
//			{
//
//				//couldn't convert values for reading
//				//perhaps write an error and return
//
//			}
//
//			//collect data from sensors 16-34
//			for (uint8 sensorNum = busLow; sensorNum <= busHigh; sensorNum++)
//			{
//				if (!TemperatureRead(sensorNum, (&values[sensorNum])))
//				{
//					//write to error log
//				}
//				else
//				{
//					values[sensorNum] = values[sensorNum] >> 4;
//				}
//			}
//
//			TemperatureRead(38, (&values[19]));
//			TemperatureRead(39, (&values[20]));
//			TemperatureRead(40, (&values[21]));
//
//			for(uint8 i = 0; i < THERMAL_SENSORS_EXO_CHECK; i++)
//			{
//				if(values[i] < 0xFF5)
//				{
//					numLow++;
//				}
//			}
//
//			if(numLow >= 22)
//			{
//				uint32 sub = (uint32) POWER_SUBSYSTEM_ACS;
//				list<VariableTypeData *> params;
//				VariableTypeData subsystem_hold(sub);
//				VariableTypeData off_hold(false);
//				VariableTypeData time_hold(time);
//				params.push_back(&subsystem_hold);
//				params.push_back(&off_hold);
//				params.push_back(&time_hold);
//				DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_POWER_SUB_CMD, params);
//			}
//#endif //HARDWARE
//
//		}
//
//	}//end namespace servers
//}//end namespace AllStar
