/*
 * 	File for COM system mode definition
 * 	Written by: Conrad Hougen
 * 	Created: 6/21/12
 */

#ifndef COMMODE_H_
#define COMMODE_H_

#include "core/SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
		class ComMode : public Phoenix::Core::SystemMode
		{
		public:
			static bool InitializeSubclass(void);
			static ComMode * GetInstance(void);

			void OnEntry(void);
			void OnExit(void);

			virtual ~ComMode( );
#ifdef HOST
			static void DestroySubclass(void);
#endif

		private:
			static ComMode * instancePtr;
			static bool isInitialized;

			ComMode( );
		};
    }
}

#endif /* COMMODE_H_ */
