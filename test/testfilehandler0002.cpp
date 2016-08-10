/* testfilehandler0001.cpp
 * Created on: Jun 18, 2014
 *    Author: pedro
 *  \brief Test 2 of the File Handler.
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

TEST(TestFileHandler, WriteAndRead)
{
	{
		FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
		size_t buffer;
		ASSERT_GE(fileHandler->FileWrite("./filehandlertest/test.txt", "test", sizeof("test")-1), (uint32) 0);
		ASSERT_GE(fileHandler->FileWrite("./filehandlertest/test1.txt", "test1\n", sizeof("test1\n")-1), (uint32) 0);
		ASSERT_GE(fileHandler->FileWrite("./filehandlertest/test1.txt", "test2\n", sizeof("test2\n")-1), (uint32) 0);
		ASSERT_GE(fileHandler->ReadFile("./filehandlertest/test.txt", &buffer), (uint8 *) 0);
	}
}
