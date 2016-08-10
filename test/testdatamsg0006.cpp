/* testdatamsg0006.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 6 of the DataMessage Class.
 *
 *  This test verifies that the GetType() method of the DataMessage
 *  class returns the correct MessageTypeEnum value.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/DataMessage.h"
#include "core/StdTypes.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE ((MessageCodeType)21)
#define TEST_VALUE1 ((double)99.9)
#define TEST_VALUE2 "super"


TEST(TestDataMessage, GetType)
{
	{
		DataMessage c;

		ASSERT_TRUE(c.GetType() == MESSAGE_TYPE_DATA);
	}
}
