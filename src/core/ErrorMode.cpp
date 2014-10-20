/*
 * ErrorMode.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/ErrorMode.h"

namespace Phoenix
{
    namespace Core
    {
		ErrorMode * ErrorMode::instancePtr = NULL;
		bool ErrorMode::isInitialized = false;

		bool ErrorMode::InitializeSubclass(void)
		{
			if (isInitialized)
			{
				return true;
			}

			if (instancePtr == NULL)
			{
				instancePtr = new ErrorMode( );
			}

			isInitialized = (instancePtr != NULL);

			if (!isInitialized)
			{
				delete instancePtr;
			}

			return isInitialized;
		}

		ErrorMode * ErrorMode::GetInstance(void)
		{
			return instancePtr;
		}

		void ErrorMode::OnEntry(void)
		{
		}

		void ErrorMode::OnExit(void)
		{
		}

		ErrorMode::~ErrorMode( )
		{
		}

#ifdef HOST
		void ErrorMode::DestroySubclass(void)
		{
			delete instancePtr;
			instancePtr = NULL;
			isInitialized = false;
		}
#endif

		ErrorMode::ErrorMode() :
				SystemMode(MODE_ERROR)
		{
			// Populate the transition table.
			for(int i = 0; i < MODE_NUM_MODES; i++)
			{
				transitionTable[i] = Permission(true);
			}
		}
    }

}
