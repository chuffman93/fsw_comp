/*
 * monitor.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "util/Logger.h"
#include <pthread.h>

//! Initializes the mutex
Lock::Lock(){
	pthread_mutex_init(&lockHandle, NULL);
}

//! Destroys the mutex
Lock::~Lock(){
	pthread_mutex_destroy(&lockHandle);
}

//! Locks the mutex
void Lock::lock(){
	pthread_mutex_lock(&lockHandle);
}

//! Unlocks the mutex
void Lock::unlock(){
	pthread_mutex_unlock(&lockHandle);
}

//! Locks the mutex for the passed lock
LockGuard::LockGuard(Lock& lock)
:lock(lock)
{
	lock.lock();
}

//! Unlocks the mutex for the passed lock
LockGuard::~LockGuard(){
	lock.unlock();
}



