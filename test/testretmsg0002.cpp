/*! \file testretmsg0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the ReturnMessage Class.
 *
 *	This test verifies that the ReturnMessage constructor
 *	with boolean and message pointer parameter correctly initialize
 *	success and messagePtr to the parameter's values.
 *
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

TEST(TestReturnMessage, BoolAndPointerConstructor) {
	VariableTypeData v1(TEST_VALUE);

	DataMessage d1(TEST_OPCODE, v1);

	// ReturnMessages to check
	ReturnMessage r1(NULL, true);
	ReturnMessage r2(&d1, false);

	// Check message pointer
	if (r1.GetMessagePtr() != NULL) {
		cout << "Constructor failed to initialize message pointer to NULL."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check success
	if (!r1.GetSuccess()) {
		cout << "Constructor failed to initialize success to parameter's value."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check message pointer
	if (!(d1 == r2.GetMessagePtr())) {
		cout
				<< "Constructor failed to initialize message pointer to parameter's value."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check success
	if (r2.GetSuccess()) {
		cout << "Constructor failed to initialize success to parameter's value."
				<< endl;

		ASSERT_TRUE(false);
	}
}
