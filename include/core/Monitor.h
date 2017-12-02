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
class Monitor{
public:
	Monitor();
	~Monitor();
protected:
	void Lock();
	void Unlock();

	/*!
	 * Guard that offers RAII-style owning of the monitor mutex
	 */
	class LockGuard{
	public:
		LockGuard(Monitor* mon);
		~LockGuard();
	private:
		Monitor* mon;
	};

private:
	//! The mutex for the instance
	pthread_mutex_t lockHandle;
};

#endif /* MONITOR_H_ */
