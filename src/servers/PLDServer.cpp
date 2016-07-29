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
		static PLDDataHandler * pldDataHandler;
		
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

		void PLDServer::Update(SystemModeEnum mode)
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
			printf("--------------------------- In loop init\n");
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
				printf("-------------------------------- Switching to startup mode\n");
				currentState = ST_STARTUP;
			}
		}


		void PLDServer::loopStartup(){
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			cdhServer->resetAssert(HARDWARE_LOCATION_PLD);
			cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
			usleep(300000);
			cdhServer->resetDeassert(HARDWARE_LOCATION_PLD);

			usleep(1000000);

			FSWPacket * turnOnScience = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_START_SCIENCE, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * ret = DispatchPacket(turnOnScience);
			if(ret->GetOpcode() != PLD_START_SCIENCE){
				//TODO Error handling
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

			FSWPacket * turnOffScience = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_STOP_SCIENCE, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * ret = DispatchPacket(turnOffScience);
			delete ret;

			usleep(1000000);

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

	} // end namespace servers
} // end namespace Phoenix
