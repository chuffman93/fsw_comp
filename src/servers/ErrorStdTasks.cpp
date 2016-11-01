/*
 * ErrorStdTasks.cpp
 *
 *  Created on: Jul 18, 2013
 *      Author: Admin
 */

#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "servers/ErrorStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/EPSStdTasks.h"
#include "HAL/Power.h"
#include "HAL/Interrupt.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <list>

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::HAL;

namespace AllStar{
namespace Servers{

void EPSError(int opcode, ACPPacket * packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	// File handler error
	if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
	{
		FileHandlerError(opcode, packet);
	}

//	switch(opcode){
//	case EPS_HS_FAILURE: //Health and Status request failure
//		if(!fileHandler->Log(SUBSYSTEM_EPS, packet)){/*do something?*/}
//		break;
//	case EPS_POWER_CYCLE_FAILURE: //power cycle request failure
//		if(!fileHandler->Log(SUBSYSTEM_EPS, packet)){/*do something?*/}
//		break;
//	default:
//		logger->Log("EPSError: Unknown error encountered!", LOGGER_LEVEL_WARN);
//		if(!fileHandler->Log(SUBSYSTEM_EPS, packet)){/*do something?*/}
//		break;
//	}
}

void COMError(int opcode, ACPPacket * packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	// File handler error
	if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END)){
		FileHandlerError(opcode, packet);
	}

//	switch(opcode)
//	{
//	case COM_HS_FAILURE: //Health and Status request failure
//		if(!fileHandler->Log(SUBSYSTEM_COM, packet)){/*do something?*/}
//		break;
//	default:
//		logger->Log("COMError: Unknown error encountered!", LOGGER_LEVEL_WARN);
//		if(!fileHandler->Log(SUBSYSTEM_COM, packet)){/*do something?*/}
//		break;
//	}
}

void PLDError(int opcode, ACPPacket * packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	// File handler error
	if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END)){
		FileHandlerError(opcode, packet);
	}

	switch(opcode){
	default:
		logger->Log("PLDError: Unknown error encountered!", LOGGER_LEVEL_WARN);
		if(!fileHandler->Log(SUBSYSTEM_PLD, packet)){/*do something?*/}
		break;
	}
}

void ACSError(int opcode, ACPPacket * packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	// File handler error
	if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END)){
		FileHandlerError(opcode, packet);
	}

	switch(opcode){
	default:
		logger->Log("ACSError: Unknown error encountered!", LOGGER_LEVEL_WARN);
		if(!fileHandler->Log(SUBSYSTEM_ACS, packet)){/*do something?*/}
		break;
	}
}

void SCHError(int opcode, ACPPacket * packet)
{
	// File handler error
	if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
	{
		FileHandlerError(opcode, packet);
	}

	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	switch(opcode)
	{
	case SCH_BUILD_SCHEDULE_FAILURE: //Health and Status request failure
		if(!fileHandler->Log(SUBSYSTEM_SCH, packet))
		{
			// write to error log
		}

		break;

	case SCH_BUILD_PLD_SCHEDULE_FAILURE: //Health and Status request failure
		if(!fileHandler->Log(SUBSYSTEM_SCH, packet))
		{
			// write to error log
		}

		break;

	case SCH_RUN_SCHEDULE_FAILURE: //Health and Status request failure
		if(!fileHandler->Log(SUBSYSTEM_SCH, packet))
		{
			// write to error log
		}

		break;
	case SCH_DEFAULT_RANGE_FAILURE:
		if(!fileHandler->Log(SUBSYSTEM_SCH, packet))
		{
			// write to error log
		}

		break;
	default:
		cout<<"ErrorStdTasks: SCHError: Unknown error encountered!"<<endl;
		if(!fileHandler->Log(SUBSYSTEM_SCH, packet))
		{
			// write to error log
		}

		break;
	}
}

void CMDError(int opcode, ACPPacket * packet)
{
	// File handler error
	if((opcode >= FILE_HANDLER_ERR_START) && (opcode <= FILE_HANDLER_ERR_END))
	{
		FileHandlerError(opcode, packet);
	}

	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	switch(opcode)
	{
	case CMD_ACP_SWITCH_BAD_SUBSYS: //Bad Subsystem Index, ACP Switch Failure
		if(!fileHandler->Log(SUBSYSTEM_CMD, packet))
		{
			// write to error log
		}

		break;
	case CMD_ACP_SWITCH_BAD_PROTOCOL: //Bad Protocol Index, ACP Switch Failure
		if(!fileHandler->Log(SUBSYSTEM_CMD, packet))
		{
			// write to error log
		}
		break;
	case CMD_ACP_SWITCH_FAILURE: //ACP Switch Failure upon packet extraction
		if(!fileHandler->Log(SUBSYSTEM_CMD, packet))
		{
			// write to error log
		}
		break;
	default:
		cout<<"ErrorStdTasks: CMDError: Unknown error encountered!"<<endl;
		if(!fileHandler->Log(SUBSYSTEM_CMD, packet))
		{
			// write to error log
		}
		break;
	}
}

void CDHError(int opcode, ACPPacket * packet)
{
	// File handler error
	if((opcode >= CDH_CMD_ERROR_MIN) && (opcode <= CDH_CMD_ERROR_MAX))
	{
		FileHandlerError(opcode, packet);
	}

	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	switch(opcode)
	{
	case CDH_CPU_USAGE_FAILURE: //Bad Subsystem Index, ACP Switch Failure
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}

		break;
	case CDH_MEM_USAGE_FAILURE:
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}
		break;
	case CDH_STORAGE_FAILURE:
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}
		break;
	case CDH_TEMP_START_FAILURE:
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}
		break;
	case CDH_TEMP_READ_FAILURE:
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}
		break;
	case CDH_HOT_SWAPS_FAILURE:
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}
		break;
	default:
		cout<<"ErrorStdTasks: CDHError: Unknown error encountered!"<<endl;
		if(!fileHandler->Log(SYSTEM_CDH, packet))
		{
			// write to error log
		}
		break;
	}
}

void FileHandlerError(int opcode, ACPPacket * packet){
	// Don't write to error log
	// This is a writing error!
	cout<<'ErrorStdTasks(): FileHandler Error!'<<endl;
}

}
}
