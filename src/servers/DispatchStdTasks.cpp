/*
 * DispatchStdTasks.cpp
 *
 * Created: 7/26/2013 11:23:00 AM
 *  Author: Admin
 */ 

#include "servers/DispatchStdTasks.h"

#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ReturnMessage.h"
#include "core/CommandMessage.h"
#include "core/DataMessage.h"
#include "core/ErrorMessage.h"
#include "core/ConfigMessage.h"

#include "util/FileHandler.h"
#include "util/Logger.h"
#include <assert.h>

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		FSWPacket * DispatchPacket(FSWPacket * query)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("DispatchStdTasks: DispatchPacket() Called with FSWPacket", LOGGER_LEVEL_DEBUG);

			//check inputs
			LocationIDType source = query->GetSource();
			LocationIDType destination = query->GetDestination();
			
			if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
				|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
			{
				logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
				FSWPacket * ret = new FSWPacket(0, PACKET_FORMAT_FAIL, false, false, MESSAGE_TYPE_ERROR);
				delete query;
				return ret;
			}
			
			//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			
			//Dispatch packet, if it fails return DISPATCH_FAILED
			if(!dispatcher->Dispatch(*query))
			{
				logger->Log("DispatchStdTasks: Failed to dispatch packet\n", LOGGER_LEVEL_WARN);
				FSWPacket * ret = new FSWPacket(0, DISPATCH_FAILED, false, false, MESSAGE_TYPE_ERROR);
				delete query;
				return ret;
			}
			//debug_led_set_led(1, LED_ON);

			DispatcherStatusEnum stat;
			// TODO: determine how to deal with response allocation
			FSWPacket * response;
			//Wait for return message, if it fails return status response from dispatcher
			logger->Log("DispatchStdTasks: Waiting for return message\n", LOGGER_LEVEL_DEBUG);
			if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*query, &response)))
			{
				logger->Log("DispatchStdTasks: Did not receive response\n", LOGGER_LEVEL_WARN);
				FSWPacket * ret = new FSWPacket(0, DISPATCHER_STATUS_ERR, false, false, MESSAGE_TYPE_ERROR);
				delete query;
				return ret;
			}

			logger->Log("DispatchStdTasks: Received return message\n", LOGGER_LEVEL_DEBUG);

			//delete query;
			assert(response != NULL);
			return response;
		}
		
//		ReturnMessage * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
//				uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode)
//		{
//			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//			logger->Log("DispatchStdTasks: DispatchPacket() Called", LOGGER_LEVEL_DEBUG);
//
//			//check inputs
//			if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
//				|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
//			{
//				logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(PACKET_FORMAT_FAIL);
//				ReturnMessage * eRet = new ReturnMessage(&err, false);
//				return eRet;
//			}
//
//			//message holder
//			Message * msg;
//			//build message based on type and opCode.
//			switch (type)
//			{
//				case MESSAGE_TYPE_COMMAND:
//					msg = new CommandMessage(opCode);
//					break;
//				case MESSAGE_TYPE_DATA:
//					msg = new DataMessage(opCode);
//					break;
//				case MESSAGE_TYPE_ERROR:
//					msg = new ErrorMessage(opCode);
//					break;
//				case MESSAGE_TYPE_CONFIG:
//					msg = new ConfigMessage(opCode);
//					break;
//				default:
//					ErrorMessage err(PACKET_FORMAT_FAIL);
//					ReturnMessage * eRet = new ReturnMessage(&err, false);
//					return eRet;
//			}
//
//			//create FSWPacket based on parameters and the created message
//			FSWPacket * query = new FSWPacket(source, destination, number, timestamp, msg);
//			//delete message, packet has duplicated it.
//			delete msg;
//
//			return(DispatchPacket(query));
//		}
		
//		FSWPacket * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
//								uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode, int testvar)
//		{
//			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//			logger->Log("DispatchStdTasks: DispatchPacket() Called", LOGGER_LEVEL_DEBUG);
//
//			//check inputs
//			if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
//				|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
//			{
//				logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(PACKET_FORMAT_FAIL);
//				ReturnMessage * eRet = new ReturnMessage(&err, false);
//				return eRet;
//			}
//
//			//message holder
//			Message * msg;
//			//build message based on type and opCode.
//			switch (type)
//			{
//				case MESSAGE_TYPE_COMMAND:
//					msg = new CommandMessage(opCode);
//					break;
//				case MESSAGE_TYPE_DATA:
//					msg = new DataMessage(opCode);
//					break;
//				case MESSAGE_TYPE_ERROR:
//					msg = new ErrorMessage(opCode);
//					break;
//				case MESSAGE_TYPE_CONFIG:
//					msg = new ConfigMessage(opCode);
//					break;
//				default:
//					ErrorMessage err(PACKET_FORMAT_FAIL);
//					ReturnMessage * eRet = new ReturnMessage(&err, false);
//					return eRet;
//			}
//
//			//create FSWPacket based on parameters and the created message
//			FSWPacket * query = new FSWPacket(source, destination, number, timestamp, msg);
//			//delete message, packet has duplicated it.
//			delete msg;
//
//			return(DispatchPacket(query));
//		}

//		Phoenix::Core::ReturnMessage * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
//				uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode, const Phoenix::Core::VariableTypeData & parameterIn)
//		{
//			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//			logger->Log("DispatchStdTasks: DispatchPacket() called", LOGGER_LEVEL_DEBUG);
//			//check inputs
//			if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
//					|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
//			{
//				logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(PACKET_FORMAT_FAIL);
//				ReturnMessage * eRet = new ReturnMessage(&err, false);
//				return eRet;
//			}
//
//			//message holder
//			Message * msg;
//			//build message based on type, opCode, parameterIn.
//			switch (type)
//			{
//				case MESSAGE_TYPE_COMMAND:
//					msg = new CommandMessage(opCode, parameterIn);
//					break;
//				case MESSAGE_TYPE_DATA:
//					msg = new DataMessage(opCode, parameterIn);
//					break;
//				case MESSAGE_TYPE_ERROR:
//					msg = new ErrorMessage(opCode, parameterIn);
//					break;
//				case MESSAGE_TYPE_CONFIG:
//					msg = new ConfigMessage(opCode, parameterIn);
//					break;
//				default:
//					ErrorMessage err(PACKET_FORMAT_FAIL);
//					ReturnMessage * eRet = new ReturnMessage(&err, false);
//					return eRet;
//			}
//
//			//create FSWPacket based on parameters and the created message
//			FSWPacket * query = new FSWPacket(source, destination, number, timestamp, msg);
//			//delete message, packet has duplicated it.
//			delete msg;
//
//			return(DispatchPacket(query));
//		}
		
//		Phoenix::Core::ReturnMessage * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
//				uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode, const std::list<Phoenix::Core::VariableTypeData*> & parametersIn)
//		{
//			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//			logger->Log("DispatchStdTasks: DispatchPacket() called", LOGGER_LEVEL_DEBUG);
//
//			//check inputs
//			if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
//			|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
//			{
//				logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
//				ErrorMessage err(PACKET_FORMAT_FAIL);
//				ReturnMessage * eRet = new ReturnMessage(&err, false);
//				return eRet;
//			}
//
//			//message holder
//			Message * msg;
//			//build message based on type, opCode, parameterIn.
//			switch (type)
//			{
//				case MESSAGE_TYPE_COMMAND:
//					msg = new CommandMessage(opCode, parametersIn);
//					break;
//				case MESSAGE_TYPE_DATA:
//					msg = new DataMessage(opCode, parametersIn);
//					break;
//				case MESSAGE_TYPE_ERROR:
//					msg = new ErrorMessage(opCode, parametersIn);
//					break;
//				case MESSAGE_TYPE_CONFIG:
//					msg = new ConfigMessage(opCode, parametersIn);
//					break;
//				default:
//					ErrorMessage err(PACKET_FORMAT_FAIL);
//					ReturnMessage * eRet = new ReturnMessage(&err, false);
//					return eRet;
//			}
//
//			//create FSWPacket based on parameters and the created message
//			FSWPacket * query = new FSWPacket(source, destination, number, timestamp, msg);
//			//delete message, packet has duplicated it.
//			delete msg;
//
//			return(DispatchPacket(query));
//		}
		
		bool ExtractParameters(const FSWPacket & packet, uint32 * inputParameters, uint32 numParams, void ** outputParameters)
		{
			MultiDataMessage * dataMessage = (dynamic_cast<MultiDataMessage *>(packet.GetMessagePtr()));
			if(dataMessage == NULL)
			{
				printf("dataMessage is null?!\n");
				outputParameters = NULL;
				return false;
			}
			
			return(ExtractParameters((*dataMessage), inputParameters, numParams, outputParameters));
		}
		
		bool ExtractParameters(const ReturnMessage & retMsg, uint32 * inputParameters, uint32 numParams, void ** outputParameters)
		{
			MultiDataMessage * dataMessage = (dynamic_cast<MultiDataMessage *>(retMsg.GetMessagePtr()));
			if(dataMessage == NULL)
			{
				outputParameters = NULL;
				return false;
			}

			return(ExtractParameters((*dataMessage), inputParameters, numParams, outputParameters));
		}
		
		bool ExtractParameters(const MultiDataMessage & dataMessage, uint32 * inputParameters, uint32 numParams, void ** outputParameters)
		{
			std::list<VariableTypeData*> params = dataMessage.GetParameters();
			if(numParams != params.size())
			{
				printf("numParams is %d : params.size() is %d\n", numParams, params.size());
				printf("num params is wrong!\n");
				outputParameters = NULL;
				return false;
			}
			
			int i;
			for(i = 0; i < numParams; i++)
			{		
				VariableTypeData * temp = params.front();
				params.pop_front();
				
				if(((VariableTypeEnum) inputParameters[i]) != temp->GetType())
				{
					printf("the type is wrong!\n");
					outputParameters = NULL;
					return false;
				}				
				outputParameters[i] = temp->GetData();
			}
			return (i == numParams);
		}
		
		uint32 GetUInt(uint8 * buffer){
			uint32 result;
			uint8 flipped[] = {buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, flipped, 4);
			return result;
		}

		uint16 GetUInt16(uint8 * buffer){
			uint32 result;
			uint8 flipped[] = {buffer[1],buffer[0]};
			memcpy(&result, flipped, 2);
			return result;
		}

		int32 GetInt(uint8 * buffer){
			int32 result;
			uint8 flipped[] = {buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, flipped, 4);
			return result;
		}

		bool GetBool(uint8 * buffer){
			return((bool) (buffer[0]));
		}

		float GetFloat(uint8 * buffer){
			float result;
			uint8 flipped[] = {buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, flipped, 4);
			return result;
		}

		double GetDouble(uint8 * buffer){
			double result;
			uint8 flipped[] = {buffer[7],buffer[6],buffer[5],buffer[4],buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, flipped, 8);
			return result;
		}

		void AddUInt32(uint8 * buffer, uint32 data){
			memcpy(buffer, &data, sizeof(uint32));
		}

		void AddFloat(uint8 * buffer, float data){
			memcpy(buffer, &data, sizeof(float));
		}

		void MessageProcess(LocationIDType id, ReturnMessage * retMsg)
		{
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("DispatchStdTasks: MessageProcess() called", LOGGER_LEVEL_DEBUG);
			
			//dispatcher error! Log it
			MultiDataMessage * dataMessage = dynamic_cast<MultiDataMessage *> (retMsg->GetMessagePtr());
			if(dataMessage == NULL)
			{
				//error
				delete retMsg;
				return;
			}
			
			bool success = retMsg->GetSuccess();
			MessageCodeType retOpcode = dataMessage->GetOpcode();
			if(!success) //error message!
			{
				logger->Log("DispatchStdTasks: Got error return message", LOGGER_LEVEL_INFO);
				if((retOpcode >= DISPATCHER_ERR_START) && (retOpcode <= DISPATCHER_ERR_END))
				{
					//dispatcher error! Log it
					if(!fileHandler->Log(SYSTEM_CDH, retOpcode, (* dataMessage)))
					{
						// write to error log
					}
					delete retMsg;
					return;
				}
				else
				{ //Non dispatcher error message, send it to the octopus!
					Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
					
					FSWPacket * query = new FSWPacket(id, SERVER_LOCATION_ERR, 1, 0, dataMessage);

					//Dispatch packet, if it fails return DISPATCH_FAILED
					if(!dispatcher->Dispatch(*query)) {
						ErrorMessage err(DISPATCH_FAILED);
						ReturnMessage * eRet = new ReturnMessage(&err, false);
						delete query;
						MessageProcess(id, eRet);
						//What to do with original error message?
						delete retMsg;
						return;
					}

					ReturnMessage tempRetMsg;
					DispatcherStatusEnum stat;
					//Wait for return message, if it fails return status response from dispatcher
					if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*query, tempRetMsg)))
					{
						ErrorMessage err(DISPATCH_FAILED);
						ReturnMessage * eRet = new ReturnMessage(&err, false);
						delete query;
						MessageProcess(id, eRet);
						//What to do with original error message?
						delete retMsg;
						return;
					}
					//successfully sent message to error octopus.
					delete query;
					delete retMsg;
					return;
				}
			}
			
			//TODO: check bounds here!

			FileHandlerIDEnum handlerID;
			switch(id)
			{
				case SERVER_LOCATION_COM :
					handlerID = SUBSYSTEM_COM;
					break;
				case SERVER_LOCATION_EPS :
					handlerID = SUBSYSTEM_EPS;
					break;
				case SERVER_LOCATION_ACS :
					handlerID = SUBSYSTEM_ACS;
					break;
				case SERVER_LOCATION_PLD :
					handlerID = SUBSYSTEM_PLD;
					break;
				case SERVER_LOCATION_GPS :
					handlerID = SUBSYSTEM_GPS;
					break;
				case SERVER_LOCATION_THM :
					handlerID = SUBSYSTEM_THM;
					break;
				case SERVER_LOCATION_SCH :
					handlerID = SUBSYSTEM_SCH;
					break;
				case SERVER_LOCATION_CMD :
					handlerID = SUBSYSTEM_CMD;
					break;
				case SERVER_LOCATION_CDH :
					handlerID = SYSTEM_CDH;
					break;
				default:
					logger->Log("DispatchStdTasks: Unknown Server!", LOGGER_LEVEL_ERROR);
					return;
			}
			
			logger->Log("DispatchStdTasks: Got successful return message!", LOGGER_LEVEL_INFO);

			//If we get here that means it wasn't an error message.
			//Log the success!
			if(!fileHandler->Log(handlerID, retOpcode, (* dataMessage)))
			{
				// write to error log
				logger->Log("DispatchStdTasks: Failed to log message", LOGGER_LEVEL_WARN);
			}
			//delete retMsg;
		}

		void PacketProcess(LocationIDType id, Phoenix::Core::FSWPacket * retPacket)
		{
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("DispatchStdTasks: MessageProcess() called", LOGGER_LEVEL_DEBUG);

			// Get packet info
			bool success = retPacket->IsSuccess();
			MessageCodeType retOpcode = retPacket->GetOpcode();

			// Check if error message
			if(!success)
			{
				logger->Log("DispatchStdTasks: Got error return message", LOGGER_LEVEL_INFO);
				if((retOpcode >= DISPATCHER_ERR_START) && (retOpcode <= DISPATCHER_ERR_END))
				{
					//dispatcher error! Log it
					if(!fileHandler->Log(SYSTEM_CDH, retPacket))
					{
						logger->Log("DispatchStdTasks: Failed to log error!", LOGGER_LEVEL_ERROR);
					}
					delete retPacket;
					return;
				}
				else
				{ //Non dispatcher error message, send it to the octopus!
					Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

					//Dispatch packet, if it fails return DISPATCH_FAILED
					if(!dispatcher->Dispatch(*retPacket)) {
						// TODO: handle this
						logger->Log("DispatchStdTasks: Failed to dispatch error to octopus", LOGGER_LEVEL_ERROR);
						delete retPacket;
						return;
					}

					DispatcherStatusEnum stat;
					FSWPacket * responsePacket;
					//Wait for return message, if it fails return status response from dispatcher
					if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*retPacket, &responsePacket)))
					{
						logger->Log("DispatchStdTasks: no response from error octopus (NOTE: check ERRServer Handler return)", LOGGER_LEVEL_ERROR);
						FSWPacket * ret = new FSWPacket(0, DISPATCH_FAILED, false, false, MESSAGE_TYPE_ERROR);
						PacketProcess(id, ret);
						delete retPacket;
						return;
					}

					logger->Log("DispatchStdTasks: error octopus now has error", LOGGER_LEVEL_DEBUG);
					delete retPacket;
					return;
				}
			}

			//TODO: check bounds here!

			FileHandlerIDEnum handlerID;
			switch(id)
			{
				case SERVER_LOCATION_COM :
					handlerID = SUBSYSTEM_COM;
					break;
				case SERVER_LOCATION_EPS :
					handlerID = SUBSYSTEM_EPS;
					break;
				case SERVER_LOCATION_ACS :
					handlerID = SUBSYSTEM_ACS;
					break;
				case SERVER_LOCATION_PLD :
					handlerID = SUBSYSTEM_PLD;
					break;
				case SERVER_LOCATION_GPS :
					handlerID = SUBSYSTEM_GPS;
					break;
				case SERVER_LOCATION_THM :
					handlerID = SUBSYSTEM_THM;
					break;
				case SERVER_LOCATION_SCH :
					handlerID = SUBSYSTEM_SCH;
					break;
				case SERVER_LOCATION_CMD :
					handlerID = SUBSYSTEM_CMD;
					break;
				case SERVER_LOCATION_CDH :
					handlerID = SYSTEM_CDH;
					break;
				default:
					logger->Log("DispatchStdTasks: Unknown Server!", LOGGER_LEVEL_ERROR);
					return;
			}

			// Successful return message
			logger->Log("DispatchStdTasks: Got successful return message!", LOGGER_LEVEL_INFO);
			if(!fileHandler->Log(handlerID, retPacket))
			{
				// write to error log
				logger->Log("DispatchStdTasks: Failed to log message", LOGGER_LEVEL_WARN);
			}
			delete retPacket;
		}
	}
}
