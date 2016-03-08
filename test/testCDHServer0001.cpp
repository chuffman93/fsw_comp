/*
 * testCDHServer0001.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */


#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "core/Factory.h"
#include "stdlib.h"
#include <stdio.h>

#include "servers/CDHServer.h"

using namespace std;
using namespace rel_ops;
//using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

TEST(TestCDHServer, testSingleton){

	Factory::GetInstance(CDH_SERVER_SINGLETON);
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	ASSERT_TRUE(cdhServer->Exist());

}

