/*
 * Singleton.cpp
 *
 * Created: 8/13/2013 12:55:50 PM
 *  Author: Admin
 */ 

#include "core/Singleton.h"
#include <stdio.h>
#include <stdlib.h>

namespace Phoenix
{
	namespace Core
	{
        bool Singleton::IsFullyInitialized()
        {

	        return (initVar == 0);
        }
        
        Singleton::Singleton( )
        {
	        initVar = pthread_mutex_init(&lockHandle, NULL );
        }
        
        Singleton::~Singleton( )
        {
	        if(!initVar)
	        {
		        pthread_mutex_destroy(&lockHandle);
	        }
        }
		bool Singleton::TakeLock(size_t xBlockTime)
		{
			return pthread_mutex_timedlock(&lockHandle, xBlockTime);
		}
		
		bool Singleton::GiveLock()
		{
			return !pthread_mutex_unlock(&lockHandle);
		}
	}
}
