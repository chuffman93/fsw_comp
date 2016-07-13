/*
* THMServer.cpp
*
*  Created on: July 09, 2012
*      Author: Caitlyn Cooke
*
*  Modified on: May 16, 2016
*  		Author: Alex St. Clair
*/

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"

#include "servers/THMServer.h"
#include "servers/THMStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "util/FileHandler.h"

#include <stdio.h>
#include <stdlib.h>


//#include "boards/backplane/dbg_led.h"

#include "HAL/Temperature.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;


namespace Phoenix
{
	namespace Servers
	{

		
		THMServer::THMServer(string nameIn, LocationIDType idIn)
			: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			/*
			UBOUND[0]= 70; UBOUND[1]= 70; UBOUND[2]= 46; UBOUND[3]= 46; UBOUND[4]= 46; UBOUND[5]= 58; UBOUND[6]= 58; UBOUND[7]= 85; UBOUND[8]= 140; UBOUND[9]= 62;
			UBOUND[10]= 54; UBOUND[11]= 100; UBOUND[12]= 98; UBOUND[13]= 105; UBOUND[14]= 115; UBOUND[15]= 98;
			LBOUND[0] = -25; LBOUND[1] = -25; LBOUND[2] = -40; LBOUND[3] = -40; LBOUND[4] = -40; LBOUND[5] = -35; LBOUND[6] = -35; LBOUND[7] = -35; LBOUND[8] = -25;
			LBOUND[9] = -75; LBOUND[10] = -60; LBOUND[11] = -10; LBOUND[12] = -20; LBOUND[13] = -20; LBOUND[14] = -20; LBOUND[15] = -20;
			*/
		}

		THMServer::~THMServer()
		{
			// Left Intentionally Blank
		}
		
		void THMServer::Initialize()
		{
			//bool rv = TemperatureInit();
		}
		
#ifdef TEST
 		void THMServer::Destroy(void)
		{
		}
#endif
		
		bool THMServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		THMServer & THMServer::operator=(const THMServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}


		void THMServer::SubsystemLoop(void)
		{	
			//ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			uint8 seconds = 0;
			for(size_t i = 0; i < 10; i++)
			{
				//wdm->Kick();
				usleep(1000000);
			}
			printf("Thermal Server Entered Subsystem Loop\n");

			while(1)
			{
				uint64_t LastTickTime = getTimeInMillis();

				//wdm->Kick();
				while(dispatcher->Listen(id));

#ifdef HARDWARE
				if(seconds % 5 == 0)
				{
					DataCollect(0);
					DataCollect(1);
					DataCollect(2);
					DataCollect(3);

					// TODO: determine if this is necessary
					/*
					if((modeManager->GetMode()) != (ComMode::GetInstance()))
					{
						IsDark();
					}
					*/
				}
#endif
								
				seconds++;
				// Delay
				waitUntil(LastTickTime, 1000);
			}
		}

		void THMServer::Update(SystemModeEnum mode)
		{
			// initialize the four buses
		}
		bool THMServer::RegisterHandlers()
		{
			return true;
		}
		
		uint16* THMServer::getUpper(void)
		{
			return UBOUND;
		}
		
		uint16* THMServer::getLower(void)
		{
			return LBOUND;
		}

	}
}
