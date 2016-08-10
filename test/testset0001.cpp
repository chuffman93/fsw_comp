/*! \file testset0001.cpp
 *  * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the Setting Class.
 *
 *	This test verifies that the default constructor
 *	of Setting correctly initializes item to 0, and data
 *	with the default constructor.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Setting.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

TEST(TestSetting, DefaultConstuctor) {
	Setting s1;

	VariableTypeData v1;

	// Check item
	if (s1.GetItem() != 0) {
		cout << "Default constructor for Setting failed to set item to 0."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check data
	if (s1.GetValue() != v1) {
		cout
				<< "Default constructor for Setting failed to initialize data with default constructor."
				<< endl;

		ASSERT_TRUE(false);
	}
}
