/*
 * SchMode.h
 *
 *  Created on: May 23, 2013
 *      Author: Brian Campuzano
 */

#ifndef SCHMODE_H_
#define SCHMODE_H_

#include "core/StdTypes.h"

namespace Phoenix
{
    namespace Servers
    {

		/// \brief Single scheduled task (Generic structure)
        class SCHMode
        {
        public:

			/// \brief Returns true if it is time to go to next task
			virtual bool CheckForSwitch(void) = 0;
			
			/// \brief Checks whether the task should switch.
			bool CheckGoToNextMode();


        protected:

			/// \brief Holds information about how the task should be switched (timed wait, GPS etc)
			ScheduleSwitchTypeEnum switchType;

			/// \brief Mode to switch to after checkForSwitch returns true
			SystemModeEnum targetMode;

        };
    }
}

#endif /* SCHMODE_H_ */
