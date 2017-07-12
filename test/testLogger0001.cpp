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
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	logger->Info("Test info");
	logger->Debug("Test debug");
	logger->Warning("Test warning");
	logger->Error("Test error");
	logger->Fatal("Test fatal");
}

TEST(TestLogger, testOverload){
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	string str = "string";
	string str2 = "Test string 2: %s";

	logger->Info("Test normal");
	logger->Info(str.c_str());
	logger->Info("Test int: %d", (int) -10);
	logger->Info("Test uint: %u", (uint32) 10);
	logger->Info("Test char[]: %s", "Test");
	logger->Info("Test string: %s", str.c_str());
	logger->Info(str2.c_str(), str.c_str());
}
