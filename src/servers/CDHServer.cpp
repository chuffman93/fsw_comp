/*
 * CDHServer.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHHandlers.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
#include <iostream>
#include <sys/sysinfo.h>

using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		static CDHCPUUsageHandler * cdhCPUUsageHandler;
		static CDHMemUsageHandler * cdhMemUsageHandler;
		static CDHStorageHandler * cdhStorageHandler;
		static CDHTempBusHandler * cdhTempBusHandler;
		static CDHHotSwapsHandler * cdhHotSwapsHandler;

		CDHServer::CDHServer(std::string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			// Left Intentionally Blank
		}

		CDHServer::~CDHServer()
		{
			// Left Intentionally Blank
		}

		CDHServer & CDHServer::operator=(const CDHServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}

		void CDHServer::Initialize(void)
		{
			cdhCPUUsageHandler = new CDHCPUUsageHandler();
			cdhMemUsageHandler = new CDHMemUsageHandler();
			cdhStorageHandler = new CDHStorageHandler();
			cdhTempBusHandler = new CDHTempBusHandler();
			cdhHotSwapsHandler = new CDHHotSwapsHandler();
		}

#ifdef TEST
		void CDHServer::Destroy(void)
		{
			delete cdhCPUUsageHandler;
			delete cdhMemUsageHandler;
			delete cdhStorageHandler;
			delete cdhTempBusHandler;
			delete cdhHotSwapsHandler;

			// TODO: Add Temperature Bus, Hot swaps, storage, storage management, memory
		}
#endif

		bool CDHServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void CDHServer::Update(const SystemMode * mode)
		{
			/* Called by mode manager each time mode changes
			 * Ex: Needs to do things to close mode 1, enter mode 2
			 * Setup and tear down between modes
			 */
		}

		/*
		 * Testing Purposes only
		 */
		bool CDHServer::Exist()
		{
			std::cout<<"CDH"<<std::endl;
			return true;
		}


		bool CDHServer::RegisterHandlers()
		{
			bool success = true;
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			// ACS Command OpCodes
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_CPU_USAGE_CMD), cdhCPUUsageHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_MEM_USAGE_CMD), cdhMemUsageHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_STORAGE_CMD), cdhStorageHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_TEMP_BUS_CMD), cdhTempBusHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_HOT_SWAPS_CMD), cdhHotSwapsHandler);

			for(int opcode = CDH_CMD_MIN; opcode < CDH_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void CDHServer::SubsystemLoop(void)
		{
			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			//ErrorQueue * que = dynamic_cast<ErrorQueue *>(Factory::GetInstance(ERROR_QUEUE_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			//Check if there are errors in the queue.
			//FSWPacket * tmpPacket;
			//size_t num = 0;
			uint64_t LastWakeTime = 0;
			while(1)
			{
				while(dispatcher->Listen(id));
				LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				//debug_led_set_led(2, LED_TOGGLE);
				/*
				num = que->ErrorsWaiting();
				while(num > 0)
				{
					printf("ErrorOctopus Errors Received!\n");
					//add error event logger here
					tmpPacket = que->GetNextError();
					LocationIDType source = tmpPacket->GetSource();
					MultiDataMessage * Msg = dynamic_cast<MultiDataMessage *> (tmpPacket->GetMessagePtr());
					MessageCodeType opcode = Msg->GetOpcode();
					//fileHandler->logAppend(LOG_ERROR, source, opcode);
					switch (source)
					{
						case HARDWARE_LOCATION_EPS:
							EPSError(opcode, Msg);
							break;
						case HARDWARE_LOCATION_ACS:
							ACSError(opcode, Msg);
							break;
						case HARDWARE_LOCATION_PROP:
							//DAFAQ is prop?
							break;
						case HARDWARE_LOCATION_THM:
							THMError(opcode, Msg);
							break;
						case HARDWARE_LOCATION_PLD:
							PLDError(opcode, Msg);
							break;
						case HARDWARE_LOCATION_GPS:
							GPSError(opcode, Msg);
							break;
						case SERVER_LOCATION_COM:
							COMError(opcode, Msg);
							break;
						case SERVER_LOCATION_EPS:
							EPSError(opcode, Msg);
							break;
						case SERVER_LOCATION_ACS:
							ACSError(opcode, Msg);
							break;
						case SERVER_LOCATION_PROP:
							//Seriously though wtf is propulsion?
							break;
						case SERVER_LOCATION_PLD:
							PLDError(opcode, Msg);
							break;
						case SERVER_LOCATION_GPS:
							GPSError(opcode, Msg);
							break;
						case SERVER_LOCATION_THM:
							printf("Thermal Error!\n");
							THMError(opcode, Msg);
							break;
						case SERVER_LOCATION_SCH:
							SCHError(opcode, Msg);
							break;
						case SERVER_LOCATION_CMD:
							CMDError(opcode, Msg);
							break;
						case SERVER_LOCATION_ERR:
							//TODO: Error server throwing errors?
							//We're gonna need a bigger boat.
							break;
						default:
							//TODO: WTF WHERE DID THIS ERROR COME FROM!
							break;
					}
					delete tmpPacket;
					//update number of messages remaining.
					num = que->ErrorsWaiting();
				}
				*/
				// Delay
				waitUntil(LastWakeTime, 1000);
			}
		}
	}
}

