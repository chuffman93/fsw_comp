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

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;
//using namespace Phoenix::Servers;


TEST(TestLogger, testSingleton){
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log("Test info", LOGGER_LEVEL_INFO);
	logger->Log("Test debug", LOGGER_LEVEL_DEBUG);
	logger->Log("Test warning", LOGGER_LEVEL_WARN);
	logger->Log("Test error", LOGGER_LEVEL_ERROR);
	logger->Log("Test fatal", LOGGER_LEVEL_FATAL);
}
