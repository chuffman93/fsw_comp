/*
 * AccessMode.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef ACCESSMODE_H_
#define ACCESSMODE_H_

#include "SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
        class AccessMode : public Phoenix::Core::SystemMode
        {
        public:
        	static bool InitializeSubclass(void);
            static AccessMode * GetInstance(void);

            void OnEntry(void);
            void OnExit(void);

            virtual ~AccessMode( );
#ifdef HOST
            static void DestroySubclass(void);
#endif

        private:
            static AccessMode * instancePtr;
            static bool isInitialized;

            AccessMode( );
        };
    }
}

#endif /* ACCESSMODE_H_ */
