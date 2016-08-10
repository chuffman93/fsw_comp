/* testcommsg0006.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 6 of the CommandMessage Class.
 *
 *  This test verifies that the GetType() method of the CommandMessage
 *  class returns the correct MessageTypeEnum value.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/CommandMessage.h"
#include "core/StdTypes.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE ((MessageCodeType)21)
#define TEST_VALUE1 ((double)99.9)
#define TEST_VALUE2 "super"


TEST(TestCommandMessage, GetType)
{
	{
		CommandMessage c;

		ASSERT_TRUE(c.GetType() == MESSAGE_TYPE_COMMAND);
	}
}
