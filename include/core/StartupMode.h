/*
 * StartupMode.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef STARTUPMODE_H_
#define STARTUPMODE_H_

#include "SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
		class StartupMode : public Phoenix::Core::SystemMode
		{
		public:
			static bool InitializeSubclass(void);
			static StartupMode * GetInstance(void);

			void OnEntry(void);
			void OnExit(void);

			virtual ~StartupMode( );
#ifdef TEST
			static void DestroySubclass(void);
#endif

		private:
			static StartupMode * instancePtr;
			static bool isInitialized;

			StartupMode( );
		};
    }
}

#endif /* ACCESSMODE_H_ */
