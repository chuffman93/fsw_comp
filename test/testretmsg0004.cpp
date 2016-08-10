/*! \file testretmsg0004.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 4 of the ReturnMessage Class.
 *
 *	This test verifies that the is response method correctly
 *	returns true for all ReturnMessages
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ReturnMessage.h"
#include "core/DataMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE 23
#define TEST_VALUE ((int32)98765)

TEST(TestReturnMessage, IsResponse) {
	ReturnMessage r1;

	VariableTypeData v1(TEST_VALUE);

	DataMessage d1(TEST_OPCODE, v1);

	ReturnMessage r2(&d1, true);

	// Check response
	if (!r1.IsResponse()) {
		cout << "Response failed to return true for ReturnMessage" << endl;

		ASSERT_TRUE(false);
	}

	// Check response
	if (!r2.IsResponse()) {
		cout << "Response failed to return true for ReturnMessage" << endl;

		ASSERT_TRUE(false);
	}
}
