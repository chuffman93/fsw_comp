/*
 * monitor.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "core/Monitor.h"
#include <pthread.h>

//! Initializes the mutex
LockClass::LockClass(){
	pthread_mutex_init(&lockHandle, NULL);
}

//! Destroys the mutex
LockClass::~LockClass(){
	pthread_mutex_destroy(&lockHandle);
}

//! Locks the mutex
void LockClass::Lock(){
	pthread_mutex_lock(&lockHandle);
}

//! Unlocks the mutex
void LockClass::Unlock(){
	pthread_mutex_unlock(&lockHandle);
}

Monitor::Monitor(LockClass* lock)
{
	this->lock = lock;
	this->lock->Lock();
}

Monitor::~Monitor(){
	this->lock->Unlock();
}



