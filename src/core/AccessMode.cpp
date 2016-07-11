/*
 * AccessMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include <iostream>

#include "core/AccessMode.h"
#include "core/Permission.h"

#include "servers/CDHServer.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
        AccessMode * AccessMode::instancePtr = NULL;
        bool AccessMode::isInitialized = false;

        bool AccessMode::InitializeSubclass(void)
		{
			if (isInitialized)
			{
				return true;
			}

			if (instancePtr == NULL)
			{
				instancePtr = new AccessMode( );
			}

			isInitialized = (instancePtr != NULL);

			if (!isInitialized)
			{
				delete instancePtr;
			}
			return isInitialized;
		}

        AccessMode * AccessMode::GetInstance(void)
        {
            return instancePtr;
        }

        void AccessMode::OnEntry(void)
        {
        	Servers::CDHServer * cdhServer = dynamic_cast<Servers::CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
        	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);
        	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
        	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
        	cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);
        }

        void AccessMode::OnExit(void)
        {
        	Servers::CDHServer * cdhServer = dynamic_cast<Servers::CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
        	cdhServer->subPowerOff(HARDWARE_LOCATION_COM);
			cdhServer->subPowerOff(HARDWARE_LOCATION_ACS);
			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
			cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
        }

        AccessMode::~AccessMode( )
        {
        }

#ifdef TEST
        void AccessMode::DestroySubclass(void)
        {
        	delete instancePtr;
        	instancePtr = NULL;
        	isInitialized = false;
        }
#endif

        AccessMode::AccessMode() :
                SystemMode(MODE_ACCESS)
        {
            // Populate the transition table.
        	for(int i = 0; i < MODE_NUM_MODES; i++)
        	{
        		transitionTable[i] = Permission(true);
        	}
        }
    }

}
