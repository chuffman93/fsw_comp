/*
 * monitor.h
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */

#ifndef MONITOR_H_
#define MONITOR_H_
#include <pthread.h>

/*!
 * Simple class that offers mutex methods and manages a mutex.
 */
class Lock{
public:
	Lock();
	~Lock();
	void lock();
	void unlock();

private:
	//! The mutex for the instance
	pthread_mutex_t lockHandle;
};


/*!
 * Guard that offers RAII-style owning of a Lock
 */
class LockGuard{
public:
	LockGuard(Lock& lock);
	~LockGuard();
private:
	//! A pointer to the lock object that is being owned by the LockGuard
	Lock& lock;
};

#endif /* MONITOR_H_ */
