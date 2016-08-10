/*! \file testsendmsg0002.cpp
 *  * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the SendMessage Class.
 *
 *	This test verifies that the getOpcode method works
 *	for all SendMessages.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

#define TEST_OPCODE1 23
#define TEST_OPCODE2 43
#define TEST_OPCODE3 62
#define TEST_OPCODE4 87

TEST(TestMessage, SendMessage)
{
	DataMessage d1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE2);
	ConfigMessage cf1(TEST_OPCODE3);
	CommandMessage cm1(TEST_OPCODE4);

	if (d1.GetOpcode() != TEST_OPCODE1 || e1.GetOpcode() != TEST_OPCODE2
		|| cf1.GetOpcode() != TEST_OPCODE3 || cm1.GetOpcode() != TEST_OPCODE4)
	{
		cout << "SendMessage did not return the correct opcode." << endl;

		ASSERT_TRUE(false);
	}
}
