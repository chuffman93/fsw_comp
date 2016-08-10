/*! \file testretmsg0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the ReturnMessage Class.
 *
 *	This test verifies that the default constructor
 *	of ReturnMessage correctly initializes the message pointer
 *	to null, and success to false.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ReturnMessage.h"

using namespace std;
using namespace AllStar::Core;

TEST(TestReturnMessage, DefaultConstructor) {

	// Default constructor
	ReturnMessage r1;

	// Check success
	if (r1.GetSuccess()) {
		cout
				<< "Default constructor for ReturnMessage failed to initialize success to false."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check message pointer
	if (r1.GetMessagePtr() != NULL) {
		cout
				<< "Default constructor for ReturnMessage failed to initialize message pointer to null."
				<< endl;

		ASSERT_TRUE(false);

	}
}
