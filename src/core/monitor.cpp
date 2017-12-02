/*
 * monitor.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "core/Monitor.h"
#include <pthread.h>

//! Initializes the mutex
Monitor::Monitor(){
	pthread_mutex_init(&lockHandle, NULL);
}

//! Destroys the mutex
Monitor::~Monitor(){
	pthread_mutex_destroy(&lockHandle);
}

//! Locks the mutex
void Monitor::Lock(){
	pthread_mutex_lock(&lockHandle);
}

//! Unlocks the mutex
void Monitor::Unlock(){
	pthread_mutex_unlock(&lockHandle);
}

Monitor::LockGuard::LockGuard(Monitor* mon)
:mon(mon)
{
	mon->Lock();
}

Monitor::LockGuard::~LockGuard(){
	mon->Unlock();
}



