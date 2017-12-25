/*
 * test_Logger.cpp
 *
 *  Created on: Dec 10, 2017
 *      Author: cyborg9
 */

#include "test/catch.hpp"
#include "util/Logger.h"
#include "util/EventTags.h"
#include <iostream>

using namespace std;

TEST_CASE("Test that logger does STDIO output", "[.][Logger][stdio]"){
	Logger logger;

	//Handle whatever level it is at
	SECTION("Test level Debug"){
		logger.setLevel(LEVEL_DEBUG);
		logger.log(LEVEL_DEBUG, "This should be printed");
		logger.log(LEVEL_INFO, "This should be printed");
		logger.log(LEVEL_WARN, "This should be printed");
		logger.log(LEVEL_ERROR, "This should be printed");
		logger.log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Info"){
		logger.setLevel(LEVEL_INFO);
		logger.log(LEVEL_DEBUG, "This should not be printed");
		logger.log(LEVEL_INFO, "This should be printed");
		logger.log(LEVEL_WARN, "This should be printed");
		logger.log(LEVEL_ERROR, "This should be printed");
		logger.log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Warn"){
		logger.setLevel(LEVEL_WARN);
		logger.log(LEVEL_DEBUG, "This should not be printed");
		logger.log(LEVEL_INFO, "This should not be printed");
		logger.log(LEVEL_WARN, "This should be printed");
		logger.log(LEVEL_ERROR, "This should be printed");
		logger.log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Error"){
		logger.setLevel(LEVEL_ERROR);
		logger.log(LEVEL_DEBUG, "This should not be printed");
		logger.log(LEVEL_INFO, "This should not be printed");
		logger.log(LEVEL_WARN, "This should not be printed");
		logger.log(LEVEL_ERROR, "This should be printed");
		logger.log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Fatal"){
		logger.setLevel(LEVEL_FATAL);
		logger.log(LEVEL_DEBUG, "This should not be printed");
		logger.log(LEVEL_INFO, "This should not be printed");
		logger.log(LEVEL_WARN, "This should not be printed");
		logger.log(LEVEL_ERROR, "This should not be printed");
		logger.log(LEVEL_FATAL, "This should be printed");
	}

}

static void* test_thread(void* arg){
	Logger* logger = reinterpret_cast<Logger*>(arg);
	logger->registerThread("Thread 2");
	logger->log(LEVEL_INFO, "Printing from thread 2");
	return NULL;
}

TEST_CASE("Logger prints thread ids", "[.][Logger]"){
	cout << "------------Beginning Logger thread test--------------" << endl;
	cout.flush();
	Logger logger;
	pthread_t thr;
	pthread_create(&thr, NULL, test_thread, &logger);

	logger.registerThread("Thread 1");

	logger.log(LEVEL_INFO, "Printing from thread 1");
	pthread_join(thr, NULL);
}

TEST_CASE("EventTags Perform correctly", "[Logger][Tags]"){
	EventTags tags1, tags2;
	tags1 += EventTag("source", "Test");
	tags2 += EventTag("newtag", "Test2");
	EventTags newtags = tags1 + tags2;
	REQUIRE(newtags["source"] == "Test");
	REQUIRE(newtags["newtag"] == "Test2");

}


