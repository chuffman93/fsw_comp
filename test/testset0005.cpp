/*! \file testset0005.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 5 of the Setting Class.
 *
 *	This test verifies that the get item and get value
 *	methods both return the correct value
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
#define TEST_VALUE2 false

#define TEST_CONFIG1 234
#define TEST_CONFIG2 876

#define BUFFER_SIZE 100

TEST(TestSetting, GetItemAndGetValue) {
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	Setting s1(TEST_CONFIG1, v1);
	Setting s2(TEST_CONFIG2, v2);

	// Check get item
	if (s1.GetItem() != TEST_CONFIG1 || s2.GetItem() != TEST_CONFIG2) {
		cout << "Get item failed to return the correct value." << endl;

		ASSERT_TRUE(false);
	}

	// Check get value
	if (s1.GetValue() != v1 || s2.GetValue() != v2) {
		cout << "Get value failed to return the correct value." << endl;

		ASSERT_TRUE(false);
	}
}
