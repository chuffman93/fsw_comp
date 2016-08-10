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

	logger->Log("Test info", LOGGER_LEVEL_INFO);
	logger->Log("Test debug", LOGGER_LEVEL_DEBUG);
	logger->Log("Test warning", LOGGER_LEVEL_WARN);
	logger->Log("Test error", LOGGER_LEVEL_ERROR);
	logger->Log("Test fatal", LOGGER_LEVEL_FATAL);
}

TEST(TestLogger, testOverload){
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	string str = "string";
	string str2 = "Test string 2: %s";

	logger->Log("Test normal", LOGGER_LEVEL_INFO);
	logger->Log(str, LOGGER_LEVEL_INFO);
	logger->Log("Test int: %d", (int) -10, LOGGER_LEVEL_INFO);
	logger->Log("Test uint: %u", (uint32) 10, LOGGER_LEVEL_INFO);
	logger->Log("Test char[]: %s", "Test", LOGGER_LEVEL_INFO);
	logger->Log("Test string: %s", str, LOGGER_LEVEL_INFO);
	logger->Log(str2, str, LOGGER_LEVEL_INFO);
}
