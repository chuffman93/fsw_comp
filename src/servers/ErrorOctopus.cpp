/*
* ErrorOctopus.cpp
*
*  Created on: Feb 27, 2013
*      Author: Brian Campuzano
*/

#include "servers/ErrorOctopus.h"
#include "servers/ErrorQueue.h"
#include "servers/ErrorStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"

#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace AllStar::Core;

namespace AllStar
{
	namespace Servers
	{
		// Error handler for EPS errors.
		static ErrorOctopusHandler * octErrorHandler;
		
		ErrorOctopus::ErrorOctopus(std::string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			// Left Intentionally Blank
		}

		ErrorOctopus::~ErrorOctopus()
		{
			// Left Intentionally Blank
		}

		ErrorOctopus & ErrorOctopus::operator=(const ErrorOctopus & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}
		
		void ErrorOctopus::Initialize(void)
		{
			octErrorHandler = new ErrorOctopusHandler();
		}
		
#ifdef HOST
		void ErrorOctopus::Destroy(void)
		{
			delete octErrorHandler;
		}
#endif
		
		bool ErrorOctopus::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}
		
		void ErrorOctopus::Update(SystemModeEnum mode)
		{
			/* Called by mode manager each time mode changes
			* Ex: Needs to do things to close mode 1, enter mode 2
			* Setup and tear down between modes
			*/
		}


		bool ErrorOctopus::RegisterHandlers()
		{
			bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			for(int opcode = OPCODE_ERRORS_MIN; opcode < OPCODE_ERRORS_MAX; opcode++)
			{
				success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), octErrorHandler);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), true);
			}
			
			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void ErrorOctopus::SubsystemLoop(void)
		{
			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			ErrorQueue * que = dynamic_cast<ErrorQueue *>(Factory::GetInstance(ERROR_QUEUE_SINGLETON));			
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			//Check if there are errors in the queue.
			FSWPacket * error;
			size_t num = 0;
			uint64_t LastWakeTime = 0;
			while(1)
			{
				while(Listen(id));
				LastWakeTime = getTimeInMillis();
				//wdm->Kick();
				//debug_led_set_led(2, LED_TOGGLE);
				num = que->ErrorsWaiting();
				while(num > 0)
				{
					printf("ErrorOctopus Errors Received!\n");
					//add error event logger here
					error = que->GetNextError();
					LocationIDType source = error->GetSource();
					MessageCodeType opcode = error->GetOpcode();
					//fileHandler->logAppend(LOG_ERROR, source, opcode);
					switch (source)
					{
						case HARDWARE_LOCATION_EPS:
							EPSError(opcode, error);
							break;
						case HARDWARE_LOCATION_ACS:
							ACSError(opcode, error);
							break;
						case HARDWARE_LOCATION_PROP:
							//DAFAQ is prop?
							break;
						case HARDWARE_LOCATION_PLD:
							PLDError(opcode, error);
							break;
						case HARDWARE_LOCATION_GPS:
							GPSError(opcode, error);
							break;
						case SERVER_LOCATION_COM:
							COMError(opcode, error);
							break;
						case SERVER_LOCATION_EPS:
							EPSError(opcode, error);
							break;
						case SERVER_LOCATION_ACS:
							ACSError(opcode, error);
							break;
						case SERVER_LOCATION_PROP:
							//Seriously though wtf is propulsion?
							break;
						case SERVER_LOCATION_PLD:
							PLDError(opcode, error);
							break;
						case SERVER_LOCATION_GPS:
							GPSError(opcode, error);
							break;
						case SERVER_LOCATION_SCH:
							SCHError(opcode, error);
							break;
						case SERVER_LOCATION_CMD:
							CMDError(opcode, error);
							break;
						case SERVER_LOCATION_CDH:
							CDHError(opcode, error);
							break;
						case SERVER_LOCATION_ERR:
							//TODO: Error server throwing errors?
							//We're gonna need a bigger boat.
							break;
						default:
							//TODO: WTF WHERE DID THIS ERROR COME FROM!
							break;
					}
					delete error;
					//update number of messages remaining.
					num = que->ErrorsWaiting();
				}
				// Delay
				waitUntil(LastWakeTime, 1000);
			}
		}
	}
}

