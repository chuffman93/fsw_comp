/*
 * PayloadPriorityMode.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef PAYLOADPRIORITYMODE_H_
#define PAYLOADPRIORITYMODE_H_

#include "SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
		class PayloadPriorityMode : public Phoenix::Core::SystemMode
		{
		public:
			static bool InitializeSubclass(void);
			static PayloadPriorityMode * GetInstance(void);

			void OnEntry(void);
			void OnExit(void);

			virtual ~PayloadPriorityMode( );
#ifdef HOST
			static void DestroySubclass(void);
#endif

		private:
			static PayloadPriorityMode * instancePtr;
			static bool isInitialized;

			PayloadPriorityMode( );
		};
    }
}

#endif /* PAYLOADPRIORITYMODE_H_ */
