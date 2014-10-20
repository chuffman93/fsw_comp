/*
 * BusPriorityMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/BusPriorityMode.h"

namespace Phoenix
{
    namespace Core
    {
		BusPriorityMode * BusPriorityMode::instancePtr = NULL;
		bool BusPriorityMode::isInitialized = false;

		bool BusPriorityMode::InitializeSubclass(void)
		{
			if (isInitialized)
			{
				return true;
			}

			if (instancePtr == NULL)
			{
				instancePtr = new BusPriorityMode( );
			}

			isInitialized = (instancePtr != NULL);

			if (!isInitialized)
			{
				delete instancePtr;
			}

			return isInitialized;
		}

		BusPriorityMode * BusPriorityMode::GetInstance(void)
		{
			return instancePtr;
		}

		void BusPriorityMode::OnEntry(void)
		{
		}

		void BusPriorityMode::OnExit(void)
		{
		}

		BusPriorityMode::~BusPriorityMode( )
		{
		}

#ifdef HOST
		void BusPriorityMode::DestroySubclass(void)
		{
			delete instancePtr;
			instancePtr = NULL;
			isInitialized = false;
		}
#endif

		BusPriorityMode::BusPriorityMode() :
				SystemMode(MODE_BUS_PRIORITY)
		{
			// Populate the transition table.
			for(int i = 0; i < MODE_NUM_MODES; i++)
			{
				transitionTable[i] = Permission(true);
			}
		}
    }

}
