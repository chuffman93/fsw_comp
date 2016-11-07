/*
 * testLogger0001.cpp
 *
 *  Created on: May 25, 2016
 *      Author: Alex St. Clair
 */

#include "util/Logger.h"
#include "gtest/gtest.h"
#include "core/StdTypes.h"
#include "core/Factory.h"
#include <time.h>
#include <string>

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;
//using namespace AllStar::Servers;


TEST(TestLogger, testSingleton){
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "Test info");
	logger->Log(LOGGER_LEVEL_DEBUG, "Test debug");
	logger->Log(LOGGER_LEVEL_WARN, "Test warning");
	logger->Log(LOGGER_LEVEL_ERROR, "Test error");
	logger->Log(LOGGER_LEVEL_FATAL, "Test fatal");
}

TEST(TestLogger, testOverload){
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	string str = "string";
	string str2 = "Test string 2: %s";

	logger->Log(LOGGER_LEVEL_INFO, "Test normal");
	logger->Log(LOGGER_LEVEL_INFO, str);
	logger->Log(LOGGER_LEVEL_INFO, "Test int: %d", (int) -10);
	logger->Log(LOGGER_LEVEL_INFO, "Test uint: %u", (uint32) 10);
	logger->Log(LOGGER_LEVEL_INFO, "Test char[]: %s", "Test");
	logger->Log(LOGGER_LEVEL_INFO, "Test string: %s", str);
	logger->Log(LOGGER_LEVEL_INFO, str2, str);
}
