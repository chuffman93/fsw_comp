/*
 * Thread.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */
#include "util/Thread.h"




ThreadID Thread::GetID(){
	return threadID;
}


int Thread::CreateThread(const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
	return pthread_create(&threadID, attr, start_routine, arg);
}


void Thread::ExitThread(void * rv){
	pthread_exit(&rv);
}

void Thread::CancelThread(ThreadID id){
	pthread_cancel(id);
}

int Thread::JoinThread(void * rv){
	return pthread_join(threadID, &rv);
}


ThreadID Thread::GetMyThreadID(){
	return pthread_self();
}
