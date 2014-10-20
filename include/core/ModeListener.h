/*
 * ModeListener.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef MODELISTENER_H_
#define MODELISTENER_H_

#include "core/SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
        class ModeListener
        {
        public:
            virtual void Update(const SystemMode * mode) = 0;
        };
    }
}

#endif /* MODELISTENER_H_ */
