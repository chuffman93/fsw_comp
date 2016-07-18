/*
 * PLDStdTasks.cpp
 * Written by: Caitlyn Cooke
 * Created: 12/1/12
 */

#include "servers/PLDStdTasks.h"
#include "servers/PLDServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#include <stdio.h>
#include <string.h>

//#include "boards/backplane/dbg_led.h"

#include <cmath>

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		FSWPacket * PLDHealthStatus(void)
		{
			FSWPacket * HSQuery = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_HS_CMD, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * HSRet = DispatchPacket(HSQuery);
			return HSRet;
		}

		FSWPacket * PLDPicture(void)
		{
			//FSWPacket * query = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 0, PLD_PIC_CMD, true, false, MESSAGE_TYPE_COMMAND);
			//return(DispatchPacket(query));
		}

		FSWPacket * PLDGetPicture(void)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDGetPicture(): Function unfinished!", LOGGER_LEVEL_FATAL);

//			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
//			if(pldServer == NULL)
//			{
//				FSWPacket * ret = new FSWPacket(0, PLD_PIC_GET_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//				return ret;
//			}
//
//			FSWPacket * datRet;
//
//			uint32 chunk, res = 0;
//			if(!(pldServer->PLDGetResolution(&res)) || !(pldServer->PLDGetChunkSize(&chunk)))
//			{
//				FSWPacket * ret = new FSWPacket(0, PLD_PIC_GET_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//				return ret;
//			}
//
//
//			//loop
//			uint32 numberOfPackets = (uint32) ceil((double) (res/chunk));
//			for(uint32 i = 0; i < numberOfPackets; i++)
//			{
//				datRet = PLDDataRequest(i);
//				if(!PLDPictureProcess(datRet))
//				{
//					FSWPacket * ret = new FSWPacket(0, PLD_PIC_GET_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//					return ret;
//				}
//			}
//			FSWPacket * ret = new FSWPacket(0, PLD_PIC_GET_SUCCESS, true, true, MESSAGE_TYPE_DATA);
//			return ret;
		}

		FSWPacket * PLDDataRequest(const uint32 & packetNum)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDDataRequest(): Function unfinished!", LOGGER_LEVEL_FATAL);
//			VariableTypeData num_holder((uint32)packetNum);
//			return(DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_COMMAND, PLD_DATA_CMD, num_holder));
		}

		// Set resolution of picture (ResX x ResY pixels)
		FSWPacket * PLDResSet(const uint32 & resX, const uint32 & resY)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDResSet(): Unfinished function", LOGGER_LEVEL_FATAL);
//			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
////			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
//
//			if(resX > MAX_RES_X || resY > MAX_RES_Y)
//			{
//				FSWPacket * ret = new FSWPacket(0, PLD_RES_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//				return ret;
//			}
//
//			VariableTypeData x_hold(resX);
//			VariableTypeData y_hold(resY);
//
//			list<VariableTypeData *> params;
//			params.push_back(&x_hold);
//			params.push_back(&y_hold);
//
//			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
//					 PLD_RES_CONFIG, params);
//
//			uint32 chunk = 0;
//
//			if(retMsg->GetSuccess())
//			{
//				if(!(pldServer->PLDSetResolution(resX*resY)) || !(pldServer->PLDGetChunkSize(&chunk)))
//				{
//					return retMsg;
//				}
//				//TODO::filehandler
//				//fileHandler->FileSizePLDPicture(resX*resY, chunk);
//			}
//
//			//caller responsible for deleting the return message.
//			return retMsg;
		}

		FSWPacket * PLDConfigChunkSize(const uint32 & chunkSize)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDConfigChunkSize(): Function unfinished!", LOGGER_LEVEL_FATAL);

//			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
//			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
//
//			if(chunkSize > MAX_CHUNK_SIZE)
//			{
//				FSWPacket * ret = new FSWPacket(0, PLD_CHUNK_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//				return ret;
//
//			}
//
//			VariableTypeData chunk_hold(chunkSize);
//
//			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
//					 PLD_CHUNK_CONFIG, chunk_hold);
//
//			uint32 res = 0;
//
//			if(retMsg->GetSuccess())
//			{
//				if(!(pldServer->PLDSetChunkSize(chunkSize)) || !(pldServer->PLDGetResolution(&res)))
//				{
//					return retMsg;
//				}
//				//TODO::filehandler
//				//fileHandler->FileSizePLDPicture(res, chunkSize);
//			}
//			//caller responsible for deleting the return message.
//			return retMsg;
		}


		FSWPacket * PLDSetGain(const uint32 & gain)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDSetGain(): Function unfinished!", LOGGER_LEVEL_FATAL);

//			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
//
//			if(gain > MAX_GAIN)
//			{
//				FSWPacket * ret = new FSWPacket(0, PLD_GAIN_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//				return ret;
//			}
//
//			VariableTypeData gain_hold(gain);
//			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
//					 PLD_GAIN_CONFIG, gain_hold);
//
//			if(retMsg->GetSuccess())
//			{
//				pldServer->PLDSetGain(gain);
//			}
//
//			return retMsg;
		}


		FSWPacket * PLDSetExpTime(const uint32 & expTime)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDSetExpTime(): Function unfinished!", LOGGER_LEVEL_FATAL);

//			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
//
//			if(expTime > MAX_EXP_TIME)
//			{
//				FSWPacket * ret = new FSWPacket(0, PLD_EXPTIME_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//				return ret;
//
//			}
//
//			VariableTypeData exp_hold(expTime);
//			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
//					PLD_EXPTIME_CONFIG, exp_hold);
//
//			if(retMsg->GetSuccess())
//			{
//				pldServer->PLDSetExpTime(expTime);
//			}
//
//			return retMsg;
		}

		FSWPacket * PLDReset()
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDStdTasks: PLDReset(): Function unfinished!", LOGGER_LEVEL_FATAL);

//			//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
//			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
//
//			//Wrap packet opcode => command message => packet
//			CommandMessage cmd(PLD_RESET_CMD);
//			FSWPacket * query = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, &cmd);
//
//			//Dispatch packet, if it fails return DISPATCH_FAILED
//			if(!dispatcher->Dispatch(*query))
//			{
//					ErrorMessage err(DISPATCH_FAILED);
//					ReturnMessage * eRet = new ReturnMessage(&err, false);
//					delete query;
//					return eRet;
//			}
//
//			ReturnMessage retMsg;
//			DispatcherStatusEnum stat;
//			//Wait for return message, if it fails return status response from dispatcher
//			if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*query, retMsg)))
//			{
//					ErrorMessage err(DISPATCHER_STATUS_ERR);
//					ReturnMessage * eRet = new ReturnMessage(&err, false);
//					delete query;
//					return eRet;
//			}
//
//			delete query;
//			//Send hardware response message back to caller
//			ReturnMessage * ret = new ReturnMessage(retMsg);
//			//caller responsible for deleting the return message.
//			return ret;
		}

	   bool PLDPictureProcess(FSWPacket * retMsg)
	   {
//			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
		   Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		   logger->Log("PLDStdTasks: PLDPictureProcess(): Unfinished function!", LOGGER_LEVEL_FATAL);

			MessageCodeType opcode = retMsg->GetOpcode();

			if(!retMsg->IsSuccess()) //error message!
			{
				PacketProcess(SERVER_LOCATION_PLD, retMsg);
				return false;
			}

			//TODO: finish
//			MultiDataMessage * msgPtr = dynamic_cast<MultiDataMessage *>(retMsg->GetMessagePtr());
//			if(msgPtr == NULL )
//			{
//				// error
//
//				delete retMsg;
//				return false;
//			}

			//TODO::filehandler
//			if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (*msgPtr)))
//			{
//				return false;
//			}
			return true;
	   }

	   // TODO: do we need this?
	   FSWPacket * PLDGetData()
	   {
		   system("~/PLD_DATA_GET.sh");

		   return NULL;
	   }
	} // end namespace servers
} // end namespace phoenix
