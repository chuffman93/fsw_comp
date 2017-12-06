/*
 * test_ACPManager.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "test/catch.hpp"
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

TEST_CASE("Monitor properly mutexes", "[Monitor]"){
	test_class test;
	pthread_t thr;
	test.flag = false;

	pthread_create(&thr, NULL, test_thread, &test);
	//Wait until test_thread is in the wait function
	while(!test.flag) usleep(1);
	//Take the lock on test
	test.Lock();
	REQUIRE(test.flag == false);
	pthread_join(thr, NULL);
}
