/*
 * ErrorMode.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef ERRORMODE_H_
#define ERRORMODE_H_

#include "SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
		class ErrorMode : public Phoenix::Core::SystemMode
		{
		public:
			static bool InitializeSubclass(void);
			static ErrorMode * GetInstance(void);

			void OnEntry(void);
			void OnExit(void);

			virtual ~ErrorMode( );
#ifdef TEST
			static void DestroySubclass(void);
#endif

		private:
			static ErrorMode * instancePtr;
			static bool isInitialized;

			ErrorMode( );
		};
    }
}

#endif /* ERRORMODE_H_ */
