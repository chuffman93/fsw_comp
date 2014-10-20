/*! \file Power.h
 *  \brief Header file for the Power interface
 *
 *  This file contains the definition for the HAL Power interface.
 */

#ifndef _POWER_H_
#define _POWER_H_

#include "core/StdTypes.h"

namespace Phoenix
{
    namespace HAL
    {
        enum Command {
            STATUS = 0,
            SET_POWER_STATE,
            GET_POWER_STATE
        };

        /*! \brief Initialize the Power interface
         *
         *  Initializes the power interface.
         *
         *  \return true if successfully initialized, false otherwise.
         */
        bool PowerInit(void);

        /*! \brief Set the power state of a subsystem
         *
         *  Turns subsystem power on/off.
         *
         *  \param subsystem Which subsystem to modify.
         *  \param on true to turn power on, false to turn power off.
         *  \return true if command successfully sent to power hardware.
         */
        bool PowerSet(uint8 subsystem, bool on);

        /*! \brief Get the power state of a subsystem
         *
         *  Gets the actual and expected power state of a subsystem
         *
         *  \param subsystem Which subsystem's state to get.
         *  \param actual Where to store actual state.
         *                true for on, false for off.
         *  \param expected Where to store expected state.
         *                  true for on, false for off.
         *  \return true if states successfully read from power hardware.
         */
        bool PowerGet(uint8 subsystem, bool *actual, bool *expected);
    }
}

#endif /* _POWER_H_ */
