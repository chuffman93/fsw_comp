/*
 * Singleton.cpp
 *
 * Created: 8/13/2013 12:55:50 PM
 *  Author: Admin
 */ 

#include "core/Singleton.h"
#include <stdio.h>
#include <stdlib.h>

namespace AllStar {
namespace Core {

bool Singleton::IsFullyInitialized() {

	return (initVar == 0);
}

Singleton::Singleton() {
	initVar = pthread_mutex_init(&lockHandle, NULL);
}

Singleton::~Singleton() {
	if (!initVar) {
		pthread_mutex_destroy(&lockHandle);
	}
}
bool Singleton::TakeLock(size_t xBlockTime) {
	return pthread_mutex_timedlock(&lockHandle, xBlockTime);
}

bool Singleton::GiveLock() {
	return !pthread_mutex_unlock(&lockHandle);
}

// Debug method to check if the lock can be taken
bool Singleton::CheckLock() {
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		this->GiveLock();
		return true;
	} else {
		return false;
	}
}

} // End of namespace Core
} // End of namespace AllStar
