/*
 * SystemMode.cpp
 *
 * ALLSTAR's system mode class.
 *
 *  Created on: Mar 10, 2012
 *      Author: Andrew Barker (adapted from Mode.cpp by Riley)
 */

#include "core/SystemMode.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/ComMode.h"

#ifdef HOST
#include "TestMode.h"
#endif

namespace Phoenix
{
    namespace Core
    {
        bool SystemMode::Initialize(void)
        {
        	if(!AccessMode::InitializeSubclass())
        	{
        		return false;
        	}
        	if(!StartupMode::InitializeSubclass())
			{
				return false;
			}
        	if(!BusPriorityMode::InitializeSubclass())
			{
				return false;
			}
        	if(!PayloadPriorityMode::InitializeSubclass())
			{
				return false;
			}
        	if(!ErrorMode::InitializeSubclass())
			{
				return false;
			}
        	if(!ComMode::InitializeSubclass())
			{
				return false;
			}

			#ifdef HOST
        	if(!TestMode::Initialize())
			{
				return false;
			}
			#endif //HOST

            return true;
        }


        bool SystemMode::CheckValidTransition(SystemModeEnum newMode, LocationIDType server) const
        {
            if (MODE_FIRST_MODE <= newMode && newMode < MODE_NUM_MODES)
            {
                return this->transitionTable[newMode].ServerHasPermission(server);
            }
            else
            {
                return false;
            }
        }

        SystemMode::SystemMode(SystemModeEnum modeIn) :
        		systemMode(modeIn)
        {
            // Left Intentionally Blank.
        }

#ifdef HOST
        void SystemMode::Destroy(void)
        {
        	AccessMode::DestroySubclass();
			StartupMode::DestroySubclass();
			BusPriorityMode::DestroySubclass();
			PayloadPriorityMode::DestroySubclass();
			ErrorMode::DestroySubclass();
			ComMode::DestroySubclass();
#ifdef HOST
			TestMode::DestroySubclass();
#endif
        }
#endif

        SystemMode::~SystemMode()
        {
        }
    }
}
