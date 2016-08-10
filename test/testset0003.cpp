/*! \file testset0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 3 of the Setting Class.
 *
 *	This test verifies that the buffer constructor
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
#define TEST_CONFIG 123

#define BUFFER_SIZE 100

TEST(TestSetting, BufferConstructor) {
	uint8 * buffer = new uint8[BUFFER_SIZE];
	uint8 * bufferStart = buffer;
	// START CREATING BUFFER

	// Put the item into the buffer (4 bytes)
	*buffer++ = (TEST_CONFIG >> 24) & 0xFF;
	*buffer++ = (TEST_CONFIG >> 16) & 0xFF;
	*buffer++ = (TEST_CONFIG >> 8) & 0xFF;
	*buffer++ = TEST_CONFIG & 0xFF;

	// Update the size
	size_t size = sizeof(ConfigItemType);

	VariableTypeData v1(TEST_VALUE);

	// Put the value into the buffer and update the size
	size += v1.Flatten(buffer, v1.GetFlattenSize());

	// Reset the buffer pointer to the beginning
	buffer = bufferStart;

	// BUFFER CREATED

	Setting s1(buffer, size);

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

	delete buffer;
}
