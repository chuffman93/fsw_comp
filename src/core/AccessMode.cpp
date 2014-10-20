/*
 * AccessMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include <iostream>
#include "core/AccessMode.h"
#include "core/Permission.h"
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
        }

        void AccessMode::OnExit(void)
        {
        }

        AccessMode::~AccessMode( )
        {
        }

#ifdef HOST
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
