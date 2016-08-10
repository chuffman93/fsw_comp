/*! \file testvtd0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 3 of the VariableTypeData Class.
 *
 *  This test verifies that the unsigned integer constructor correctly sets
 *  the data and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

#define TEST_VALUE ((uint32)5)

TEST(TestVariableTypeData, UnsignedIntegerConstuctor) {
	VariableTypeData v1(TEST_VALUE);

	if (v1.GetType() != VAR_TYPE_ENUM_UNSIGNED_INT) {
		ASSERT_TRUE(false);
	}
	if (*((uint32 *) v1.GetData()) != TEST_VALUE) {
		ASSERT_TRUE(false);
	}
}
