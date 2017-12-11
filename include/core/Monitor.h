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
 * Partial implementation of a monitor structure
 */
class LockClass{
public:
	LockClass();
	~LockClass();
protected:
	void Lock();
	void Unlock();

	/*!
	 * Guard that offers RAII-style owning of the monitor mutex
	 */

private:
	//! The mutex for the instance
	pthread_mutex_t lockHandle;
};


class Monitor{
public:
	Monitor(LockClass* lock);
	~Monitor();
private:
	LockClass* lock;
};

#endif /* MONITOR_H_ */
