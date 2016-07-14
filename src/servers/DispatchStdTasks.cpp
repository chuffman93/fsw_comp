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

			//grab and increment the packet number
			query->SetNumber(dispatcher->packetNumber++);
			
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
			FSWPacket * response;
			//Wait for return message, if it fails return status response from dispatcher
			logger->Log("DispatchStdTasks: Waiting for return message\n", LOGGER_LEVEL_DEBUG);
			if(DISPATCHER_STATUS_OK != (stat = WaitForDispatchResponse(*query, &response)))
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

		void PacketProcess(LocationIDType id, Phoenix::Core::FSWPacket * retPacket)
		{
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("DispatchStdTasks: PacketProcess() called", LOGGER_LEVEL_DEBUG);

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
					if(DISPATCHER_STATUS_OK != (stat = WaitForDispatchResponse(*retPacket, &responsePacket)))
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
					delete retPacket;
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

		DispatcherStatusEnum WaitForDispatchResponse(const FSWPacket & packet, FSWPacket ** retPacketin)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			size_t i;
			FSWPacket * retPacket;
			logger->Log("    Dispatcher: WaitForDispatchResponse() called", LOGGER_LEVEL_DEBUG);
			logger->Log("    Dispatcher: Checking queue for matching packet", LOGGER_LEVEL_DEBUG);
			for (i = 0; i < DISPATCHER_MAX_RESPONSE_TRIES; ++i)
			{
				if (dispatcher->CheckQueueForMatchingPacket(packet, retPacket, dispatcher->CHECK_MATCHING_RESPONSE))
				{
					logger->Log("    Dispatcher: WaitForDispatchResponse(): Matching FSWPacket found.", LOGGER_LEVEL_DEBUG);

					*retPacketin = retPacket;
					return DISPATCHER_STATUS_OK;
				}
				usleep(DISPATCHER_WAIT_TIME);
			}

			// At this point, see if the command we sent has been received at least.
			logger->Log("   Dispatch:  No response, see if the packet we sent has been received.", LOGGER_LEVEL_DEBUG);
			//debug_led_set_led(6, LED_ON);

			if (dispatcher->CheckQueueForMatchingPacket(packet, retPacket, dispatcher->CHECK_SAME))
			{
				logger->Log("   Dispatch: Command not received, removed from queue", LOGGER_LEVEL_ERROR);
				return DISPATCHER_STATUS_MSG_NOT_RCVD;
			}

			// The command was received, but no response has been placed in
			// the queue, so return that the operation failed.
			logger->Log("   Dispatch: Command received, but no response sent", LOGGER_LEVEL_ERROR);
			return DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT;
		}

		uint32 GetUInt(uint8 * buffer){
			uint32 result;
			uint8 littleEndian[] = {buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, littleEndian, 4);
			return result;
		}

		uint16 GetUInt16(uint8 * buffer){
			uint32 result;
			uint8 littleEndian[] = {buffer[1],buffer[0]};
			memcpy(&result, littleEndian, 2);
			return result;
		}

		int32 GetInt(uint8 * buffer){
			int32 result;
			uint8 littleEndian[] = {buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, littleEndian, 4);
			return result;
		}

		bool GetBool(uint8 * buffer){
			return((bool) (buffer[0]));
		}

		float GetFloat(uint8 * buffer){
			float result;
			uint8 littleEndian[] = {buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, littleEndian, 4);
			return result;
		}

		double GetDouble(uint8 * buffer){
			double result;
			uint8 littleEndian[] = {buffer[7],buffer[6],buffer[5],buffer[4],buffer[3],buffer[2],buffer[1],buffer[0]};
			memcpy(&result, littleEndian, 8);
			return result;
		}

		void AddUInt32(uint8 * buffer, uint32 data){
			memcpy(buffer, &data, sizeof(uint32));
		}

		void AddFloat(uint8 * buffer, float data){
			memcpy(buffer, &data, sizeof(float));
		}

		void AddDouble(uint8 * buffer, double data){
			memcpy(buffer, &data, sizeof(double));
		}
	}
}
