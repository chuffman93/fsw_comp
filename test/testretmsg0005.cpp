/*! \file testretmsg0005.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 5 of the ReturnMessage Class.
 *
 *	This test verifies that the get and set success methods
 *	are working correctly.
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

TEST(TestReturnMessage, GetSetSuccess) {
	ReturnMessage r1;

	VariableTypeData v1(TEST_VALUE);

	DataMessage d1(TEST_OPCODE, v1);

	ReturnMessage r2(&d1, true);

	// Check success
	if (r1.GetSuccess() || !r2.GetSuccess()) {
		cout << "Get success failed to return the right value." << endl;

		ASSERT_TRUE(false);
	}

	r1.SetSuccess(true);
	r2.SetSuccess(false);

	// Check success
	if (!r1.GetSuccess() || r2.GetSuccess()) {
		cout << "Set success failed to set the right value." << endl;

		ASSERT_TRUE(false);
	}
}
