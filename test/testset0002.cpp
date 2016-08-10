/*! \file testset0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the Setting Class.
 *
 *	This test verifies that the constructor with ConfigItemType
 *	and VariableTypeData parameters correctly initalizes item and data
 *	to the parameter's values.
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

TEST(TestSetting, Constructor2) {
	VariableTypeData v1(TEST_VALUE);

	Setting s1(TEST_CONFIG, v1);

	// Check item
	if (s1.GetItem() != TEST_CONFIG) {
		cout
				<< "Default constructor for Setting failed to set item to parameter's value."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check data
	if (s1.GetValue() != v1) {
		cout
				<< "Default constructor for Setting failed to initialize data with parameter's value."
				<< endl;

		ASSERT_TRUE(false);
	}
}
