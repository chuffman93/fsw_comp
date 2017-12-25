/*
 * Thread.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_UTIL_THREAD_H_
#define INCLUDE_UTIL_THREAD_H_


#include <pthread.h>


typedef pthread_t ThreadID;


class Thread
{
public:

	Thread(){
		threadID = 0; //TODO what is good initial value?
	}

	~Thread(){};

	ThreadID GetID();
	static ThreadID GetMyThreadID();
	int CreateThread(const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
	void ExitThread(void * rv);
	int JoinThread(void * rv);
	static void CancelThread(ThreadID id);

private:
	ThreadID threadID;
};





#endif /* INCLUDE_UTIL_THREAD_H_ */
