/*
 * HAL.h
 *
 *  Created on: Oct 25, 2010
 *  Author: Frank Dischner
 */

#ifndef _HAL_H_
#define _HAL_H_

namespace Phoenix
{
    namespace HAL
    {
        /*! \brief Initialize OS elements for HAL interface
         *
         *  Calls the various OSInit functions to initialize
         *  the HAL interfaces.
         */
        void HALInit(void);
    }
}

#endif /* _HAL_H_ */
