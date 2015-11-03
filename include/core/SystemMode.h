/*
 * SystemMode.h
 *
 * ALLSTAR's system mode class.
 *
 *  Created on: Mar 9, 2012
 *      Author: Andrew Barker (adapted from Mode.h by Riley)
 */

#ifndef SYSTEMMODE_H_
#define SYSTEMMODE_H_

#include "core/StdTypes.h"
#include "core/Permission.h"

namespace Phoenix
{
    namespace Core
    {


        class SystemMode
        {
        public:

            static bool Initialize(void);

            virtual void OnEntry(void) = 0;
            virtual void OnExit(void) = 0;

            SystemModeEnum GetID(void) const
            {
                return systemMode;
            }

            bool CheckValidTransition(SystemModeEnum newMode, LocationIDType server) const;

            virtual ~SystemMode(void);
#ifdef TEST
            static void Destroy(void);
#endif

        protected:
            SystemModeEnum systemMode;
            Permission transitionTable[MODE_NUM_MODES];

            SystemMode(SystemModeEnum modeIn = MODE_ACCESS);
        };
    }
}

#endif /* SYSTEMMODE_H_ */
