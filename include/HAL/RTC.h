/*! \file RTC.h
 *  \brief Header file for the RTC interface
 *
 *  This file contains the definition for the HAL RTC interface.
 */

#ifndef _RTC_H_
#define _RTC_H_

#include "core/StdTypes.h"

namespace AllStar
{
    namespace HAL
    {
        /*! \brief Initialize OS elements for RTC interface
         *
         *  Creates semaphores which are used in other RTC functions.
         *
         *  \note This function must be called before any other RTC
         *  functions and may only be called exactly once.
         */
        void RTCOSInit(void);

        /*! \brief Initialize the RTC device
         *
         *  Initializes the RTC hardware.
         *
         *  \note This function must be called before accessing the
         *  device for the first time. It may be called multiple times
         *  to reinitialize the hardware.
         *
         *  \return true if device successfully initialized, false otherwise.
         */
        bool RTCInit(void);

        /*! \brief Get current time
         *
         *  Gets the current time since epoch.
         *
         *  \param seconds where to store number of seconds since epoch or
         *  or NULL if not desired.
         *  \param count where to store sub-second count or NULL if not
         *  desired.
         *  \return true if time successfully read, false otherwise.
         */
        bool RTCGetTime(uint32 *seconds, uint16 *count);

        /*! \brief Set current time
         *
         *  Sets the current time since epoch.
         *
         *  \param Number of seconds since epoch.
         *  \return true if new time successfully set, false otherwise.
         */
        bool RTCSetTime(uint32 seconds);
    }
}

#endif /* _RTC_H_ */
