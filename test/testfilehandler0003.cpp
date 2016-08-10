/* testfilehandler0001.cpp
 * Created on: Jun 18, 2014
 *    Author: pedro
 *  \brief Test 3 of the File Handler.
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

using namespace std;
using namespace AllStar::Core;

TEST(TestFileHandler, Delete)
{
	{
		FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
		size_t buffer;
		ASSERT_TRUE(fileHandler->DeleteFile("./filehandlertest/test.txt"));
	}
}
