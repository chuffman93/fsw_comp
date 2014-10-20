/*
 * BusPriorityMode.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef BUSPRIORITYMODE_H_
#define BUSPRIORITYMODE_H_

#include "SystemMode.h"

namespace Phoenix
{
    namespace Core
    {
		class BusPriorityMode : public Phoenix::Core::SystemMode
		{
		public:
			static bool InitializeSubclass(void);
			static BusPriorityMode * GetInstance(void);

			void OnEntry(void);
			void OnExit(void);

			virtual ~BusPriorityMode( );
#ifdef HOST
			static void DestroySubclass(void);
#endif

		private:
			static BusPriorityMode * instancePtr;
			static bool isInitialized;

			BusPriorityMode( );
		};
    }
}

#endif /* BUSPRIORITYMODE_H_ */
