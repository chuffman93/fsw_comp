/*! \file Singleton.h
 *  \brief Header File for the Singleton Class
 *
 *  This file contains the definition of the Singleton class, which is
 *	a generic class implementation of a singleton
 */


#ifndef SINGLETON_H_
#define SINGLETON_H_
#include "POSIX.h"

namespace AllStar
{
	namespace Core
	{		
        class Singleton
        {
			/*! \brief Declare Factory a friend class 
			*	
			*	This allows factory to call IsFullyIntiailized and Deconstructor
			*/
			friend class Factory;
			
        public:
			bool CheckLock(void);

		protected:
			Singleton();
			virtual bool IsFullyInitialized(void);
			virtual ~Singleton(void);
			bool TakeLock(size_t xBlockTime);
			bool GiveLock(void);

		private:
			pthread_mutex_t lockHandle;
			int initVar;
        };
	}
}
#endif /* SINGLETON_H_ */
