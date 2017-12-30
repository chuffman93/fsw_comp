/*
 * test_Logger.cpp
 *
 *  Created on: Dec 10, 2017
 *      Author: cyborg9
 */

#include "test/catch.hpp"
#include "util/Logger.h"
#include <iostream>

using namespace std;

TEST_CASE("Test that Logger does STDIO output", "[.][Logger][stdio]"){
	Logger::setMode(MODE_PRINT);
	//Handle whatever level it is at
	SECTION("Test level Debug"){
		Logger::setLevel(LEVEL_DEBUG);
		Logger::log(LEVEL_DEBUG, "This should be printed");
		Logger::log(LEVEL_INFO, "This should be printed");
		Logger::log(LEVEL_WARN, "This should be printed");
		Logger::log(LEVEL_ERROR, "This should be printed");
		Logger::log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Info"){
		Logger::setLevel(LEVEL_INFO);
		Logger::log(LEVEL_DEBUG, "This should not be printed");
		Logger::log(LEVEL_INFO, "This should be printed");
		Logger::log(LEVEL_WARN, "This should be printed");
		Logger::log(LEVEL_ERROR, "This should be printed");
		Logger::log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Warn"){
		Logger::setLevel(LEVEL_WARN);
		Logger::log(LEVEL_DEBUG, "This should not be printed");
		Logger::log(LEVEL_INFO, "This should not be printed");
		Logger::log(LEVEL_WARN, "This should be printed");
		Logger::log(LEVEL_ERROR, "This should be printed");
		Logger::log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Error"){
		Logger::setLevel(LEVEL_ERROR);
		Logger::log(LEVEL_DEBUG, "This should not be printed");
		Logger::log(LEVEL_INFO, "This should not be printed");
		Logger::log(LEVEL_WARN, "This should not be printed");
		Logger::log(LEVEL_ERROR, "This should be printed");
		Logger::log(LEVEL_FATAL, "This should be printed");
	}

	SECTION("Test level Fatal"){
		Logger::setLevel(LEVEL_FATAL);
		Logger::log(LEVEL_DEBUG, "This should not be printed");
		Logger::log(LEVEL_INFO, "This should not be printed");
		Logger::log(LEVEL_WARN, "This should not be printed");
		Logger::log(LEVEL_ERROR, "This should not be printed");
		Logger::log(LEVEL_FATAL, "This should be printed");
	}
	Logger::setMode(MODE_NOTHING);
	Logger::setLevel(LEVEL_DEBUG);
}

static void* test_thread(void* arg){
	LogTags tags = *(LogTags*) arg;
	Logger::registerThread("T2");
	Logger::log(LEVEL_INFO, tags, "Printing from thread 2");
	return NULL;
}

TEST_CASE("Logger prints thread ids", "[.][Logger]"){
	Logger::setMode(MODE_PRINT);
	LogTags tags;
	tags += LogTag("Name", "Test");
	tags += LogTag("Instance", "Threads");
	Logger::log(LEVEL_FATAL, tags, "------------Beginning Logger Tread test--------------");
	pthread_t thr;
	pthread_create(&thr, NULL, test_thread, &tags);

	Logger::registerThread("T1");

	Logger::log(LEVEL_INFO, tags, "Printing from thread 1");
	pthread_join(thr, NULL);
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("Logger Stream works correctly", "[.][Logger]"){
	Logger::setMode(MODE_PRINT);
	LogTags tags = LogTag("Name", "Test");
	tags += LogTag("Instance", "Stream");
	Logger::log(LEVEL_FATAL, tags, "------------Beginning Logger Stream test--------------");
	for(int i = 0; i < 10; i++){
		Logger::Stream(LEVEL_DEBUG, tags) << "Writing from stream: " << i << ", " << 10 - i;
	}
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("Logger allows for filtering", "[.][Logger]"){
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_FATAL);
	SECTION("Single item filter"){
		Logger::log(LEVEL_FATAL, "------------Beginning Logger Single filter test--------------");
		Logger::registerFilter(LogTag("Name", "Super"), LEVEL_DEBUG);
		Logger::log(LEVEL_DEBUG, "Should not print");
		Logger::log(LEVEL_DEBUG, LogTag("Name", "NotSuper"), "Should not print");
		Logger::log(LEVEL_DEBUG, LogTag("Name", "Super"), "Should print");
	}

	SECTION("Multiple item filter"){
		Logger::log(LEVEL_FATAL, "------------Beginning Logger Multi filter test--------------");
		LogTags tagSuper = LogTag("Name", "Super");
		LogTags tagCool = LogTag("Instance", "Cool");
		Logger::registerFilter(tagSuper + tagCool, LEVEL_DEBUG);
		Logger::log(LEVEL_DEBUG, "Should not print");
		Logger::log(LEVEL_DEBUG, LogTag("Name", "NotSuper"), "Should not print");
		Logger::log(LEVEL_DEBUG, tagSuper + LogTag("Instance", "NotCool"), "Should not print");
		Logger::log(LEVEL_DEBUG, tagCool + LogTag("Name", "NotSuper"), "Should not print");
		Logger::log(LEVEL_DEBUG, tagSuper + tagCool, "Should print");
	}

	Logger::setLevel(LEVEL_DEBUG);
	Logger::clearFilters();
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("LogTags Perform correctly", "[Logger][Tags]"){
	LogTags tags1, tags2;
	tags1 += LogTag("source", "Test");
	tags2 += LogTag("newtag", "Test2");
	LogTags newtags = tags1 + tags2;
	REQUIRE(newtags["source"] == "Test");
	REQUIRE(newtags["newtag"] == "Test2");

}


