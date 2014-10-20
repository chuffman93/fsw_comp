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
		ReturnMessage * PLDHealthStatus(void)
		{
			return(DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_COMMAND, PLD_HS_CMD));
		}

		ReturnMessage * PLDPicture(void)
		{
			ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_COMMAND, PLD_PIC_CMD);
			return(ret);
		}

		ReturnMessage * PLDGetPicture(void)
		{
			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
			if(pldServer == NULL)
			{
				ErrorMessage err(PLD_PIC_GET_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			ReturnMessage * datRet;

			uint32 chunk, res = 0;
			if(!(pldServer->PLDGetResolution(&res)) || !(pldServer->PLDGetChunkSize(&chunk)))
			{
				ErrorMessage err(PLD_PIC_GET_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}


			//loop
			uint32 numberOfPackets = (uint32) ceil((double) (res/chunk));
			for(uint32 i = 0; i < numberOfPackets; i++)
			{
				datRet = PLDDataRequest(i);
				if(!PLDPictureProcess(datRet))
				{
					ErrorMessage err(PLD_PIC_GET_FAILURE);
					ReturnMessage * ret = new ReturnMessage(&err, false);
					return ret;
				}
			}
			DataMessage dat(PLD_PIC_GET_SUCCESS);
			ReturnMessage * ret = new ReturnMessage(&dat, true);
			return ret;
		}

		ReturnMessage * PLDDataRequest(const uint32 & packetNum)
		{
			VariableTypeData num_holder((uint32)packetNum);
			return(DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_COMMAND, PLD_DATA_CMD, num_holder));
		}

		// Set resolution of picture (ResX x ResY pixels)
		ReturnMessage * PLDResSet(const uint32 & resX, const uint32 & resY)
		{
			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
//			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

			if(resX > MAX_RES_X || resY > MAX_RES_Y)
			{
				ErrorMessage err(PLD_RES_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			VariableTypeData x_hold(resX);
			VariableTypeData y_hold(resY);

			list<VariableTypeData *> params;
			params.push_back(&x_hold);
			params.push_back(&y_hold);

			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
					 PLD_RES_CONFIG, params);

			uint32 chunk = 0;

			if(retMsg->GetSuccess())
			{
				if(!(pldServer->PLDSetResolution(resX*resY)) || !(pldServer->PLDGetChunkSize(&chunk)))
				{
					return retMsg;
				}
				//TODO::filehandler
				//fileHandler->FileSizePLDPicture(resX*resY, chunk);
			}

			//caller responsible for deleting the return message.
			return retMsg;
		}

		ReturnMessage * PLDConfigChunkSize(const uint32 & chunkSize)
		{
			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));
			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

			if(chunkSize > MAX_CHUNK_SIZE)
			{
				ErrorMessage err(PLD_CHUNK_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			VariableTypeData chunk_hold(chunkSize);

			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
					 PLD_CHUNK_CONFIG, chunk_hold);

			uint32 res = 0;

			if(retMsg->GetSuccess())
			{
				if(!(pldServer->PLDSetChunkSize(chunkSize)) || !(pldServer->PLDGetResolution(&res)))
				{
					return retMsg;
				}
				//TODO::filehandler
				//fileHandler->FileSizePLDPicture(res, chunkSize);
			}
			//caller responsible for deleting the return message.
			return retMsg;
		}


		ReturnMessage * PLDSetGain(const uint32 & gain)
		{
			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));

			if(gain > MAX_GAIN)
			{
				ErrorMessage err(PLD_GAIN_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			VariableTypeData gain_hold(gain);
			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
					 PLD_GAIN_CONFIG, gain_hold);

			if(retMsg->GetSuccess())
			{
				pldServer->PLDSetGain(gain);
			}

			return retMsg;
		}


		ReturnMessage * PLDSetExpTime(const uint32 & expTime)
		{
			PLDServer * pldServer = dynamic_cast<PLDServer *>(Factory::GetInstance(PLD_SERVER_SINGLETON));

			if(expTime > MAX_EXP_TIME)
			{
				ErrorMessage err(PLD_EXPTIME_FAILURE);
				ReturnMessage * ret = new ReturnMessage(&err, false);
				return ret;
			}

			VariableTypeData exp_hold(expTime);
			ReturnMessage * retMsg = DispatchPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, MESSAGE_TYPE_CONFIG,
					PLD_EXPTIME_CONFIG, exp_hold);

			if(retMsg->GetSuccess())
			{
				pldServer->PLDSetExpTime(expTime);
			}

			return retMsg;
		}

		ReturnMessage * PLDReset()
		{
			//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			//Wrap packet opcode => command message => packet
			CommandMessage cmd(PLD_RESET_CMD);
			Packet * query = new Packet(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 1, 0, &cmd);

			//Dispatch packet, if it fails return DISPATCH_FAILED
			if(!dispatcher->Dispatch(*query))
			{
					ErrorMessage err(DISPATCH_FAILED);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					delete query;
					return eRet;
			}

			ReturnMessage retMsg;
			DispatcherStatusEnum stat;
			//Wait for return message, if it fails return status response from dispatcher
			if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*query, retMsg)))
			{
					ErrorMessage err(DISPATCHER_STATUS_ERR);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					delete query;
					return eRet;
			}

			delete query;
			//Send hardware response message back to caller
			ReturnMessage * ret = new ReturnMessage(retMsg);
			//caller responsible for deleting the return message.
			return ret;
		}

	   bool PLDPictureProcess(ReturnMessage * retMsg)
	   {
//			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

			MessageCodeType opcode = retMsg->GetOpcode();

			if(!retMsg->GetSuccess()) //error message!
			{
				MessageProcess(SERVER_LOCATION_PLD, retMsg);
				return false;
			}

			MultiDataMessage * msgPtr = dynamic_cast<MultiDataMessage *>(retMsg->GetMessagePtr());
			if(msgPtr == NULL )
			{
				// error

				delete retMsg;
				return false;
			}

			//TODO::filehandler
//			if(!fileHandler->Append(SUBSYSTEM_PLD, opcode, (*msgPtr)))
//			{
//				return false;
//			}
			return true;
	   }
	   ReturnMessage * PLDGetData()
	   {
		   system("~/PLD_DATA_GET.sh");

		   return NULL;
	   }
	} // end namespace servers
} // end namespace phoenix
