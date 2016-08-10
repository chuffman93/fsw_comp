/* testfilehandler0004.cpp
 * Created on: Jul 07, 2014
 * Author: Trevor Arrasmith
 * \brief Test logAppend in FileHandler
 */

#include <iostream>
#include "gtest/gtest.h"

#include "util/FileHandler.h"
#include "core/StdTypes.h"

using namespace std;
using namespace AllStar::Core;

TEST(TestFileHandler, LogAppend)
{
	{
		FileHandler *fh = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

		ASSERT_TRUE(fh->LogAppend(LOG_MODE, 0xbb, 0x10));
		ASSERT_TRUE(fh->LogAppend(LOG_MODE, 0xcc, 0x21));
		ASSERT_TRUE(fh->LogAppend(LOG_ERROR, 0xff, 0xfe));
		ASSERT_TRUE(fh->LogAppend(LOG_MODE, 0xdd, 0x32));
	}
}
