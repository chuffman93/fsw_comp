/*
 * ErrorStdTasks.cpp
 *
 *  Created on: Jul 18, 2013
 *      Author: Admin
 */

#include <list>

#include "core/StdTypes.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"
#include "core/ModeManager.h"
#include "core/Message.h"
#include "core/ReturnMessage.h"
#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"



#include "servers/ErrorStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/EPSStdTasks.h"

#include "HAL/Power.h"
#include "HAL/Interrupt.h"

#include "util/FileHandler.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;


namespace Phoenix
{
    namespace Servers
    {
		void EPSError(int opcode, MultiDataMessage * dataMessage)
		{
			// File handler error
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				//FileHandlerError(opcode, dataMessage);
			}

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			
			switch(opcode)
			{
				case EPS_HS_FAILURE: //Health and Status request failure
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case EPS_SOC_FAILURE: //State of Charge request failure
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;

				case EPS_POWER_CYCLE_FAILURE: //power cycle request failure
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_DISABLE_OC_PROT_FAILURE: //Disable overcurrent protection failure
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_ENABLE_OC_PROT_FAILURE: //Enable overcurrent protection failure
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_OVERCURRENT_ERR: //Over-current rail trip
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_OVERVOLTAGE_ERR: //Over-voltage
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_LOW_VOLTAGE_ERR: //Low voltage
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_HIGH_TEMP_ERR: //High temperature
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_LOW_TEMP_ERR: //Low temperature
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				case EPS_BATMAN_NO_ARRAY: //batshit
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
				default:
					//PANIC!
					//TODO::filehandler
					if(!fileHandler->Append(SUBSYSTEM_EPS, opcode, (* dataMessage)))
					{
						// write to error log
					}
					break;
			}
		}

		void COMError(int opcode, MultiDataMessage * dataMessage)
		{
			// File handler error
			//TODO::filehandler
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				//TODO::filehandler
				FileHandlerError(opcode, dataMessage);
			}

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			
			switch(opcode)
			{
				case COM_HS_FAILURE: //Health and Status request failure
					if(!fileHandler->Append(SUBSYSTEM_COM, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case COM_BEACON_FAILURE: //Beacon request failure
					if(!fileHandler->Append(SUBSYSTEM_COM, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case COM_DATA_SEND_FAILURE: //Beacon rate request failure
					if(!fileHandler->Append(SUBSYSTEM_COM, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case COM_LOGIN_FAILURE: //Kermit packet send request failure
					if(!fileHandler->Append(SUBSYSTEM_COM, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case COM_RESET_FAILURE: //File downlink command failure
					if(!fileHandler->Append(SUBSYSTEM_COM, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case COM_DATA_RECEIVE_FAILURE: //File uplink command failure
					if(!fileHandler->Append(SUBSYSTEM_COM, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				default :
					//panic!
					break;
			}
		}

		void PLDError(int opcode, MultiDataMessage * dataMessage)
		{
			// File handler error
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				//FileHandlerError(opcode, dataMessage);
			}
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));		
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			
			switch(opcode)
			{
				case PLD_HS_FAILURE: //Health and Status request failure
				if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case PLD_PIC_FAILURE: //Picture command failure
				if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case PLD_DATA_FAILURE: //data request failure
				if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case PLD_RESET_FAILURE: //data request failure
				if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case PLD_CMOS_READ_FAILURE: //Failure to read CMOS
				if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case PLD_MEM_ERROR: //Local memory error
				if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				default :
					// Panic
					break;
			}
		}

		void ACSError(int opcode, MultiDataMessage * dataMessage)
		{
			// File handler error
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				//FileHandlerError(opcode, dataMessage);
			}

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			
			switch(opcode)
			{
				case ACS_HS_FAILURE: //Health and Status request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_STATE_FAILURE: //Current state request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_RAW_ADC_FAILURE: //RAW ADC request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_ATTITUDE_ERROR_REQUEST_FAILURE: //Distance from required attitude failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_POINT_MRP_VECTOR_FAILURE: //MRP point request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_POINT_GND_VECTOR_FAILURE: //Ground station point request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_REQUEST_MRP_VECTOR_FAILURE: //MRP point request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_REQUEST_GND_VECTOR_FAILURE: //Ground station point request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_SUNSOAK_FAILURE: //Sunsoak vector request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_POINT_NADIR_FAILURE: //Point NADAR failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_RANGE_TO_TARGET_FAILURE: //Range request
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_HOLD_FAILURE: //Hold request failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_DISABLE_FAILURE: //MRP vector failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_RESET_FAILURE: //Ground vector failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_GPS_FAILURE: //GPS failure
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_GYRO_STUCK_ERR: //Gyroscope stuck
				if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
				{
					// write to error log
				}

					break;
				case ACS_NO_MOTOR_RESP_ERR: //Motor not responding
					if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case ACS_CAMERA_SATURATED_ERR: //Starcamera saturation
					if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case ACS_MAG_STUCK_ERR: //Magnetometer stuck
					if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case ACS_ATTITUDE_NOT_CONVERGE_ERR: //Attitude not converging
					if(!fileHandler->Append(SUBSYSTEM_ACS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				default :
					break;
			}
		}

		void GPSError(int opcode, MultiDataMessage * dataMessage)
		{
			// File handler error
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				//FileHandlerError(opcode, dataMessage);
			}

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

			switch(opcode)
			{
				case GPS_HS_FAILURE: //Health and Status request failure
					if(!fileHandler->Append(SUBSYSTEM_GPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case GPS_TIME_FAILURE: //Time request failure
					if(!fileHandler->Append(SUBSYSTEM_GPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case GPS_POSITION_FAILURE: //Position request failure
					if(!fileHandler->Append(SUBSYSTEM_GPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case GPS_RESET_FAILURE: //Position request failure
					if(!fileHandler->Append(SUBSYSTEM_GPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case GPS_INSUFFICIENT_SATELLITES: //Insufficient satellites
					if(!fileHandler->Append(SUBSYSTEM_GPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				case GPS_INACURATE_DATA: //Inaccurate data
					if(!fileHandler->Append(SUBSYSTEM_GPS, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				default :
					break;
			}
		}


		void THMError(int opcode, MultiDataMessage * dataMessage)
		{
			uint32 enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT};
			uint32 sensor;
			uint32 temp;
			uint32 time = 60000;
			uint8 hotOrNot;
			void * outputArray[3] = {NULL};
			// File handler error
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				FileHandlerError(opcode, dataMessage);
			}

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

			switch(opcode)
			{
			case THM_HS_FAILURE: //Thermal measurement failure
				if(!fileHandler->Append(SUBSYSTEM_THM, opcode, (* dataMessage)))
				{
					// write to error log
				}
				
				if(!ExtractParameters(* dataMessage, enumArray, 3, outputArray))
				{
					return;
				}
				
				sensor = * (uint32 *) outputArray[0];
				temp = * (uint32 *) outputArray[1];
				hotOrNot = *(uint8 *) outputArray[2];
				
					
				if(hotOrNot == 1)
				{
					//THMPowerReset(/*whichpowersub?*/);
					if(sensor > 15)
					{
						return;
					}
					if(sensor == 7 || sensor == 12 || sensor == 13 || sensor == 14)
					{
						//THMPowerReset(POWER_SUBSYSTEM_ACS);
						uint32 sub = (uint32) POWER_SUBSYSTEM_ACS;
						
						VariableTypeData subsystem_hold(sub);
						VariableTypeData off_hold(false);
						VariableTypeData time_hold(time);
						
						list<VariableTypeData *> params;
						params.push_back(&subsystem_hold);
						params.push_back(&off_hold);
						params.push_back(&time_hold);
						DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_POWER_SUB_CMD, params);
					}
					if(sensor == 6 || sensor == 8 || sensor == 11)
					{
						//THMPowerReset(POWER_SUBSYSTEM_COM);
						uint32 sub = (uint32) POWER_SUBSYSTEM_COM;
						
						VariableTypeData subsystem_hold(sub);
						VariableTypeData off_hold(false);
						VariableTypeData time_hold(time);
						
						list<VariableTypeData *> params;
						params.push_back(&subsystem_hold);
						params.push_back(&off_hold);
						params.push_back(&time_hold);
						DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_POWER_SUB_CMD, params);
					}
					
				}
				
				break;
			default :
				break;

			}
		}
		
		
		void SCHError(int opcode, MultiDataMessage * dataMessage)
		{
			// File handler error
			if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
			{
				FileHandlerError(opcode, dataMessage);
			}

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

			switch(opcode)
			{
				case SCH_BUILD_SCHEDULE_FAILURE: //Health and Status request failure
					if(!fileHandler->Append(SUBSYSTEM_SCH, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				
				case SCH_BUILD_PLD_SCHEDULE_FAILURE: //Health and Status request failure
					if(!fileHandler->Append(SUBSYSTEM_SCH, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				
				case SCH_RUN_SCHEDULE_FAILURE: //Health and Status request failure
					if(!fileHandler->Append(SUBSYSTEM_SCH, opcode, (* dataMessage)))
					{
						// write to error log
					}

					break;
				default:
					break;
			}
		}

		void FileHandlerError(int opcode, MultiDataMessage * retMsg)
		{
			// Don't write to error log
			// This is a writing error!

		}

		bool SendEPSReset()
		{
			// Send message to subsystem server to reset subsystem hardware.
			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ERR, SERVER_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_POWER_CYCLE_CMD);
			if(ret->GetSuccess() == true)
			{
				return true;
			}
			return false;
		}

		bool SendCOMReset()
		{
			// Send message to subsystem server to reset subsystem hardware.
			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ERR, SERVER_LOCATION_COM, 1, 0, MESSAGE_TYPE_COMMAND, COM_RESET_CMD);
			if(ret->GetSuccess() == true)
			{
				return true;
			}
			return false;
		}

		bool SendPLDReset()
		{
			// Send message to subsystem server to reset subsystem hardware.
			//ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ERR, SERVER_LOCATION_PLD, 1, 0, MESSAGE_TYPE_COMMAND, PLD_RESET_CMD);
//			if(ret->GetSuccess() == true)
//			{
//				return true;
//			}
//			return false;
		}

		bool SendACSReset()
		{
			// Send message to subsystem server to reset subsystem hardware.
			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ERR, SERVER_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_RESET_CMD);
			if(ret->GetSuccess() == true)
			{
				return true;
			}
			return false;
		}

		bool SendGPSReset()
		{
			// Send message to subsystem server to reset subsystem hardware.
			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ERR, SERVER_LOCATION_GPS, 1, 0, MESSAGE_TYPE_COMMAND, GPS_RESET_CMD);
			if(ret->GetSuccess() == true)
			{
				return true;
			}
			return false;
		}
    }
}
