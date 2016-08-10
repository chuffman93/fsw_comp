/*! \file testretmsg0007.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 7 of the ReturnMessage Class.
 *
 *	This test verifies that the GetOpcode method correctly returns
 *	the type of message stored in the message pointer for any type
 *	of message stored there.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ReturnMessage.h"
#include "core/DataMessage.h"
#include "core/ErrorMessage.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE1 23
#define TEST_OPCODE2 78
#define TEST_OPCODE3 8
#define TEST_OPCODE4 27

TEST(TestReturnMessage, GetOpCode) {
	DataMessage d1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE2);
	ConfigMessage cf1(TEST_OPCODE3);
	CommandMessage cm1(TEST_OPCODE4);

	ReturnMessage r1(&d1, true);
	ReturnMessage r2(&e1, false);
	ReturnMessage r3(&cf1, true);
	ReturnMessage r4(&cm1, false);

	// Check opcode
	if (r1.GetOpcode() != TEST_OPCODE1 || r2.GetOpcode() != TEST_OPCODE2
			|| r3.GetOpcode() != TEST_OPCODE3 || r4.GetOpcode() != TEST_OPCODE4) {
		cout << "Get opcode failed to return the right value." << endl;

		ASSERT_TRUE(false);
	}
}
