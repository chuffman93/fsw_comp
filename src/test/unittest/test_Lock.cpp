/*
 * test_ACPManager.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "test/catch.hpp"
#include <iostream>
#include "core/Lock.h"
#include <pthread.h>
#include <unistd.h>
using namespace std;

static bool flag;

static void* test_thread(void* arg){
	Lock* test = reinterpret_cast<Lock*>(arg);
	LockGuard lock(*test);
	flag = true;
	usleep(1000);
	flag = false;
	return NULL;
}

TEST_CASE("Lock properly mutexes", "[lock]"){
	Lock lock;
	pthread_t thr;
	flag = false;

	pthread_create(&thr, NULL, test_thread, &lock);
	//Wait until test_thread is in the wait function
	while(!flag) usleep(1);
	//Take the lock on test
	lock.lock();
	REQUIRE(flag == false);
	pthread_join(thr, NULL);
}
