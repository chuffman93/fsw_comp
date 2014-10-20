/*
 * PayloadPriorityMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/PayloadPriorityMode.h"

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
		}

		void PayloadPriorityMode::OnExit(void)
		{
		}

		PayloadPriorityMode::~PayloadPriorityMode( )
		{
		}

#ifdef HOST
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
