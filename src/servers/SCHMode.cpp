/*
 * SchMode.cpp
 *
 *  Created on: May 23, 2013
 *      Author: Brian Campuzano
 */

#include "servers/SCHMode.h"

#include "core/ModeManager.h"
#include "core/Factory.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;

namespace Phoenix
{
    namespace Servers
    {
		bool SCHMode::CheckGoToNextMode()
		{
			if(this->CheckForSwitch())
			{
				//TODO: check permissions
				//Mode
				ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

				if(!modeManager->SetMode(targetMode, SERVER_LOCATION_SCH))
				{
					return false;
				}
				//debug_led_set_led(7, LED_ON);
				return true;
			}
			return false;
		}
    }
}
