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

TEST(TestLogger, testLog){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char * message = (char *) "message";
	Logger_Level level = LEVEL_INFO;

	//logger->Log(message, level);
}
