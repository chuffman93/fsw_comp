/*
* PayloadServer.cpp
*
*  Created on: May 27, 2011
*      Author: Michael
*      Last Updated: 11/23/12
*      Last Updated by: Glenda
*/

#include "servers/PLDServer.h"
#include "servers/PLDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/ComMode.h"
#include "core/SystemMode.h"

#include "util/FileHandler.h"
#include "util/Logger.h"
#include "servers/CDHServer.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		// Message handler for a health and status command.
		static PLDHSHandler * pldHSHandler;
		
		PLDServer::PLDServer(string nameIn, LocationIDType idIn) :
				SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			//TODO: default gain and exposure
		}

		PLDServer::~PLDServer()
		{
			// Left Intentionally Blank
		}

		PLDServer & PLDServer::operator=(const PLDServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}
		
		void PLDServer::Initialize(void)
		{
			pldHSHandler = new PLDHSHandler();
		}
		
#ifdef TEST
		void PLDServer::Destroy(void)
		{
			delete pldHSHandler;
		}
#endif
		
		bool PLDServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void PLDServer::Update(const SystemMode * mode)
		{
		}

		bool PLDServer::RegisterHandlers()
		{
			bool success = true;
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			// Register PLD handlers

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, PLD_HS_CMD), pldHSHandler);

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void PLDServer::loopInit(){
			//No need to do anything
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			if(modeManager->GetMode() != MODE_STARTUP){
				//Wait until we are not in startup mode, then transition to IDLE
				currentState = ST_IDLE;
			}
		}

		void PLDServer::loopIdle(){
			//No need to do anything
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			//Wait until we are in payload priority mode
			if(modeManager->GetMode() == MODE_PLD_PRIORITY){
				currentState = ST_STARTUP;
			}
		}


		void PLDServer::loopStartup(){
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

			cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
			//TODO handling reseting payload

			FSWPacket * turnOnScience = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 0, PLD_START_SCIENCE, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * ret = DispatchPacket(turnOnScience);
			if(ret->opcode != PLD_START_SCIENCE){
				//TODO Error handlinge
				currentState = ST_IDLE;
			}
			delete ret;
			//PLD is on and in science mode. Goto science loop
			currentState = ST_SCIENCE;
		}

		//Graceful shutdown
		void PLDServer::loopShutdown(){
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

			FSWPacket * turnOffScience = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 0, PLD_STOP_SCIENCE, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * ret = DispatchPacket(turnOffScience);
			delete ret;

			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
			//PLD is off. Goto idle loop
			currentState = ST_IDLE;

		}
		void PLDServer::loopScience(){
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			//Stay in science mode until we move out of PLD_PRIORITY
			if(modeManager->GetMode() != MODE_PLD_PRIORITY){
				currentState = ST_SHUTDOWN;
			}
		}

		void PLDServer::PLDAccessMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDServer: Entered Access Mode!", LOGGER_LEVEL_INFO);

			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = AccessMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 timeUnit = 1;

			FSWPacket * HSRet;
			
			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				//while(dispatcher->Listen(id));

				if(timeUnit % 60 == 0){
					timeUnit = 0;
				}

				if(timeUnit % 10 == 0){
					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);
				}

 				timeUnit++;
				waitUntil(LastWakeTime, 1000);
				currentMode = modeManager->GetMode();
			}
		}


		void PLDServer::PLDStartupMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDServer: Entered Startup Mode!", LOGGER_LEVEL_INFO);

			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = StartupMode::GetInstance();
			const SystemMode * currentMode = mode;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void PLDServer::PLDBusMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDServer: Entered Bus Mode!", LOGGER_LEVEL_INFO);

			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = BusPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));
				seconds++;
				if ((seconds % 60) == 0 )
				{
					// Functions

					//HSRet = PLDHealthStatus();
					//PacketProcess(SERVER_LOCATION_PLD, HSRet);

					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}

		}

		void PLDServer::PLDPayloadMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDServer: Entered Payload Mode!", LOGGER_LEVEL_INFO);

			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = PayloadPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;
			FSWPacket * HSRet;
			//system("echo 0 > \"/sys/class/gpio/pioE11/value\"");
			usleep(10000);
			system("echo 1 > \"/sys/class/gpio/pioE11/value\"");
			usleep(3000000);

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				seconds++;
				if ((seconds % 1) == 0 )
				{
					// Functions

					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);
					
					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				// Check current mode
				currentMode = modeManager->GetMode();
			}

//			FSWPacket * turnOffPayload = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 0, PLD_STOP_SCIENCE, true, false, MESSAGE_TYPE_COMMAND);
//			DispatchPacket(turnOffPayload);
		}

		void PLDServer::PLDErrorMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDServer: Entered Error Mode!", LOGGER_LEVEL_INFO);

			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ErrorMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Check current mode
				currentMode = modeManager->GetMode();

				seconds++;

				if ((seconds % 60) == 0 )
				{
					// Functions

					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);

					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void PLDServer::PLDComMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("PLDServer: Entered COM Mode!", LOGGER_LEVEL_INFO);

			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ComMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				seconds++;
				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);

					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();

			}
		}

	} // end namespace servers
} // end namespace Phoenix
