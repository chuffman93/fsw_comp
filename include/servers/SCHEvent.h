#ifndef SCHEVENT_H_
#define SCHEVENT_H_

#include "core/StdTypes.h"

namespace Phoenix
{
    namespace Servers
    {
		/// \brief Single scheduled event (Generic structure)
        class SCHEvent
        {
        public:
			/// \brief Returns true if it is time to go to next task
			virtual bool CheckForEvent(void) = 0;

			/// \brief Checks whether the task should switch.
			bool CheckGoToNextEvent();


        protected:

			/// \brief Holds information about how the task should be switched (timed wait, GPS etc)
			PayloadScheduleEventTypeEnum switchType;

			/// \brief subsystem to execute to after checkForSwitch returns true
			ServerLocationIDEnum subsystem;

			/// \brief opcode to execute to after checkForSwitch returns true
			uint32 opcode;

        };
    }
}

#endif /* SCHEVENT_H_ */
