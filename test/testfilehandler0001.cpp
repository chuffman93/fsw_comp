/* testfilehandler0001.cpp
 * Created on: Jun 18, 2014
 *    Author: pedro
 *  \brief Test 1 of the File Handler.
 *
 *
 */
#include <list>
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
#include "core/DataMessage.h"
#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "util/FileHandler.h"
#include "HAL/RTC.h"

using namespace std;
using namespace AllStar::Core;

TEST(TestFileHandler, Append)
{
	{
		// MAY NEED SUDO ACCESS TO RUN. YOU CAN DO THIS BY RUNNING
		// "sudo ./Test" in the correct directory, instead of running it through eclipse

		system("sudo rm -rf ./filehandlertest");
		system("mkdir ./filehandlertest");

		FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

		list<VariableTypeData *> params;
		VariableTypeData holder = VariableTypeData((uint32) 1);
		params.push_back(&holder);
		CommandMessage dat(THM_HS_CMD, params);
		CommandMessage dat2(EPS_HS_CMD, params);
		ASSERT_TRUE(fileHandler->Append(SUBSYSTEM_THM, THM_HS_CMD, dat));
		ASSERT_TRUE(fileHandler->Append(SUBSYSTEM_EPS, EPS_HS_CMD, dat2));
		ASSERT_TRUE(fileHandler->Append(SUBSYSTEM_THM, THM_HS_CMD, dat));
		ASSERT_TRUE(fileHandler->Append(SUBSYSTEM_THM, THM_HS_CMD, dat));
	}
}
