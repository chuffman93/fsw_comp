/*! \file testset0004.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 4 of the Setting Class.
 *
 *	This test verifies that the copy constructor and
 *	assignment operators both make deep copies of the source.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Setting.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

#define TEST_VALUE "wow"
#define TEST_CONFIG 234

#define BUFFER_SIZE 100

TEST(TestSetting, CopyAndAssignment) {
	VariableTypeData v1(TEST_VALUE);

	Setting s1(TEST_CONFIG, v1);

	// Copy constructor
	Setting s2 = s1;

	// Assignment operator
	Setting s3;
	s3 = s1;

	// Check copy constructor
	if (s2 != s1) {
		cout << "Copy constructor failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check assignment operator
	if (s3 != s1) {
		cout << "Assignment operator failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}
}
