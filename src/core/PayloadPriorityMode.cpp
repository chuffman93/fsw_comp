/*
 * PayloadPriorityMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/PayloadPriorityMode.h"

#include "servers/CDHServer.h"

namespace Phoenix
{
    namespace Core
    {
		PayloadPriorityMode * PayloadPriorityMode::instancePtr = NULL;
		bool PayloadPriorityMode::isInitialized = false;

		bool PayloadPriorityMode::InitializeSubclass(void)
		{
			if (isInitialized)
			{
				return true;
			}

			if (instancePtr == NULL)
			{
				instancePtr = new PayloadPriorityMode( );
			}

			isInitialized = (instancePtr != NULL);

			if (!isInitialized)
			{
				delete instancePtr;
			}

			return isInitialized;
		}

		PayloadPriorityMode * PayloadPriorityMode::GetInstance(void)
		{
			return instancePtr;
		}

		void PayloadPriorityMode::OnEntry(void)
		{
			Servers::CDHServer * cdhServer = dynamic_cast<Servers::CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

			// Subsystems on in this mode
			cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
			cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
			cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);

			// Subsystems off in this mode
			cdhServer->subPowerOff(HARDWARE_LOCATION_COM);
		}

		void PayloadPriorityMode::OnExit(void)
		{
			Servers::CDHServer * cdhServer = dynamic_cast<Servers::CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

			// Turn RAD off and COM back on
			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
			cdhServer->subPowerOn(HARDWARE_LOCATION_COM);
		}

		PayloadPriorityMode::~PayloadPriorityMode( )
		{
		}

#ifdef TEST
		void PayloadPriorityMode::DestroySubclass(void)
		{
			delete instancePtr;
			instancePtr = NULL;
			isInitialized = false;
		}
#endif

		PayloadPriorityMode::PayloadPriorityMode() :
				SystemMode(MODE_PLD_PRIORITY)
		{
			// Populate the transition table.
			for(int i = 0; i < MODE_NUM_MODES; i++)
			{
				transitionTable[i] = Permission(true);
			}
		}
    }

}
