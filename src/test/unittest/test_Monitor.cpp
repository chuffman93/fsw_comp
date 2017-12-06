/*
 * test_ACPManager.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include <iostream>
#include "core/Monitor.h"
#include <pthread.h>
#include <unistd.h>
using namespace std;

class test_class: public Monitor{
public:
	bool flag;
	void wait(){
		LockGuard lock(this);
		flag = true;
		usleep(1000);
		flag = false;
	}
	void Lock(){
		Monitor::Lock();
	}
};

static void* test_thread(void* arg){
	test_class* test = reinterpret_cast<test_class*>(arg);
	test->wait();
	return NULL;
}

void test_Monitor(){
	cout << "Monitor " << endl;
	test_class test;
	pthread_t thr;
	test.flag = false;

	pthread_create(&thr, NULL, test_thread, &test);
	//Wait until test_thread is in the wait function
	while(!test.flag) usleep(1);
	//Take the lock on test
	test.Lock();
	if(test.flag == true){
		//Allowed to run before thread1 finished, show error
		cout << "Failed to wait until the end of thread1 to complete!" << endl;
	}else{
		cout << "." << endl;
	}

	pthread_join(thr, NULL);
}

