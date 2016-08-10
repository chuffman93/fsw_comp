/*! \file testset0007.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 7 of the Setting Class.
 *
 *	This test verifies that the get flatten method works
 *	correctly for a Setting with no parameters.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Setting.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

#define BUFFER_SIZE 100

TEST(TestSetting, GetFlattenSizeNoParameters) {
	uint8 * buffer = new uint8[BUFFER_SIZE];

	// Setting to be flattened
	Setting s1in;

	// Try to flatten with a null buffer
	size_t numCopied = s1in.Flatten(NULL, s1in.GetFlattenSize());

	// Make sure that a null buffer returns 0
	if (numCopied != 0) {
		cout << "Flatten method did return 0 when the buffer "
				"parameter was null.";

		ASSERT_TRUE(false);
	}

	// Try to flatten with insufficiently large size
	numCopied = s1in.Flatten(buffer, s1in.GetFlattenSize() - 1);

	// Make sure that small size returns 0
	if (numCopied != 0) {
		cout << "Flatten method did return 0 when the size was "
				"less than GetFlattenSize().";

		ASSERT_TRUE(false);
	}

	// Flatten the setting
	numCopied = s1in.Flatten(buffer, s1in.GetFlattenSize());

	// Make sure flatten returns the flatten size
	if (numCopied != s1in.GetFlattenSize()) {
		cout << "Flatten method did return GetFlattenSize().";

		ASSERT_TRUE(false);
	}

	// Create new setting from the buffer
	Setting s1out(buffer, s1in.GetFlattenSize());

	// Check that flattened and unflattened setting is equal to original
	if (!(s1in == s1out)) {
		cout << "Flatten did not correctly preserve setting.";

		ASSERT_TRUE(false);
	}

	delete buffer;
}
