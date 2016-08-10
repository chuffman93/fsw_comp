/*! \file testset0006.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 6 of the Setting Class.
 *
 *	This test verifies that the get flatten size method
 *	works correctly.
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

TEST(TestSetting, GetFlattenSize) {
	Setting s1;
	VariableTypeData v1;

	// Calculate expected size
	size_t expectedSize = sizeof(ConfigItemType) + v1.GetFlattenSize();

	// Get actual size
	size_t actualSize = s1.GetFlattenSize();

	// Check that sizes are the same
	if (expectedSize != actualSize) {
		cout << "GetFlattenSize() method of SingleDataMessage class "
				"did not return correct size." << " GetFlattenSize( ): "
				<< actualSize << " Expected: " << expectedSize << endl;

		ASSERT_TRUE(false);
	}

	VariableTypeData v2(TEST_VALUE);

	Setting s2(TEST_CONFIG, v2);

	// Calculate expected size
	expectedSize = sizeof(ConfigItemType) + v2.GetFlattenSize();

	// Get actual size
	actualSize = s2.GetFlattenSize();

	// Check that sizes are the same
	if (expectedSize != actualSize) {
		cout << "GetFlattenSize() method of SingleDataMessage class "
				"did not return correct size." << " GetFlattenSize( ): "
				<< actualSize << " Expected: " << expectedSize << endl;

		ASSERT_TRUE(false);
	}
}
