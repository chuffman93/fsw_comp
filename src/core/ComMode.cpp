/*
 *  Implementation of the COM system mode used for communication with ground
 * 	Written by: Conrad Hougen
 * 	Created: 6/21/12
 */


#include "core/ComMode.h"

namespace Phoenix
{
    namespace Core
    {
		ComMode * ComMode::instancePtr = NULL;
		bool ComMode::isInitialized = false;

		bool ComMode::InitializeSubclass(void)
		{
			if (isInitialized)
			{
				return true;
			}

			if (instancePtr == NULL)
			{
				instancePtr = new ComMode( );
			}

			isInitialized = (instancePtr != NULL);

			if (!isInitialized)
			{
				delete instancePtr;
			}

			return isInitialized;
		}

		ComMode * ComMode::GetInstance(void)
		{
			return instancePtr;
		}

		void ComMode::OnEntry(void)
		{
		}

		void ComMode::OnExit(void)
		{
		}

		ComMode::~ComMode( )
		{

		}

#ifdef TEST
		void ComMode::DestroySubclass(void)
		{
			delete instancePtr;
			instancePtr = NULL;
			isInitialized = false;
		}
#endif

		ComMode::ComMode() :
				SystemMode(MODE_COM)
		{
			// Populate the transition table.
			for(int i = 0; i < MODE_NUM_MODES; i++)
			{
				transitionTable[i] = Permission(true);
			}
		}
    }

}
