/*
 * StartupMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/StartupMode.h"

#include "servers/CDHServer.h"

namespace Phoenix
{

    namespace Core
    {
		StartupMode * StartupMode::instancePtr = NULL;
		bool StartupMode::isInitialized = false;

		bool StartupMode::InitializeSubclass(void)
		{
			if (isInitialized)
			{
				return true;
			}

			if (instancePtr == NULL)
			{
				instancePtr = new StartupMode( );
			}

			isInitialized = (instancePtr != NULL);

			if (!isInitialized)
			{
				delete instancePtr;
			}

			return isInitialized;
		}

		StartupMode * StartupMode::GetInstance(void)
		{
			return instancePtr;
		}

		void StartupMode::OnEntry(void)
		{
			Servers::CDHServer * cdhServer = dynamic_cast<Servers::CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

			// all subsystems off in startup
			cdhServer->subPowerOff(HARDWARE_LOCATION_COM);
			cdhServer->subPowerOff(HARDWARE_LOCATION_ACS);
			cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
		}

		void StartupMode::OnExit(void)
		{
		}

		StartupMode::~StartupMode( )
		{
		}

#ifdef TEST
		void StartupMode::DestroySubclass(void)
		{
			delete instancePtr;
			instancePtr = NULL;
			isInitialized = false;
		}
#endif

		StartupMode::StartupMode() :
				SystemMode(MODE_STARTUP)
		{
			// Populate the transition table.
			for(int i = 0; i < MODE_NUM_MODES; i++)
			{
				transitionTable[i] = Permission(true);
			}
		}
    }

}
