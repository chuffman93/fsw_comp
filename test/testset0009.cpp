/*! \file testset0009.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 9 of the Setting Class.
 *
 *	This test verifies that the get == operator
 *	correctly compares two setting objects.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Setting.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

#define TEST_VALUE1 "wow"
#define TEST_VALUE2 ((double)2.2)

#define TEST_CONFIG1 234
#define TEST_CONFIG2 432

TEST(TestSetting, EqualsEquals) {
	// Default constructor
	Setting s1;
	Setting s2;

	// Check settings with no parameters
	if (s1 != s2) {
		cout << "The == operator failed to correctly compare two settings."
				<< endl;

		ASSERT_TRUE(false);
	}

	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	// Constructor with ConfigItemType and VariableTypeData parameters
	Setting s3(TEST_CONFIG1, v1);
	Setting s4(TEST_CONFIG1, v1);
	Setting s5(TEST_CONFIG1, v2);
	Setting s6(TEST_CONFIG2, v1);
	Setting s7(TEST_CONFIG2, v2);

	// Check settings with parameters
	if (s3 != s4 || s3 == s5 || s3 == s6 || s3 == s7) {
		cout << "The == operator failed to correctly compare two settings."
				<< endl;

		ASSERT_TRUE(false);
	}
}
