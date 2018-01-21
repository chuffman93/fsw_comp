/*
 * test_Thread.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */



#include "test/catch.hpp"
#include "core/Watchdog.h"
#include "util/Thread.h"
#include <iostream>
#include <unistd.h>
#include "FSWThreads.h"

using namespace std;

void * TestThreadNormal(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	watchdog->KickWatchdog();
	while(1){
		usleep(1000);
		watchdog->KickWatchdog();
	}
	return NULL;
}

void * TestThreadHang(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	while(1);
	return NULL;
}

//TODO: Fix adams shitty test case
TEST_CASE("Watchdog test", "[.][Watchdog]"){
	cout << "------------Beginning Watchdog Test--------------" << endl;
	cout.flush();

	// Create Health and Status Thread
	Thread testThread1, testThread2, watchdogThread;
	Watchdog watchdog;
	int rv = 0;

	//Handle whatever level it is at
	SECTION("Test watchdog"){
		testThread1.CreateThread(NULL, TestThreadNormal, (void*)&watchdog);
		testThread2.CreateThread(NULL, TestThreadNormal, (void*)&watchdog);
		watchdog.AddThread(testThread1.GetID());
		watchdog.AddThread(testThread2.GetID());
		watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);
		// Join Threads
		sleep(5);
		Thread::CancelThread(watchdogThread.GetID());
	}
	//Handle whatever level it is at
	SECTION("Test watchdog hanging"){
		testThread1.CreateThread(NULL, TestThreadNormal, (void*)&watchdog);
		testThread2.CreateThread(NULL, TestThreadHang, (void*)&watchdog);
		watchdog.AddThread(testThread1.GetID());
		watchdog.AddThread(testThread2.GetID());
		watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);
		// Join Threads
		sleep(5);
		Thread::CancelThread(watchdogThread.GetID());
	}
}





