/*! \file testvtd0004.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 4 of the VariableTypeData Class.
 *
 *  This test verifies that the boolean constructor correctly sets
 *  the data and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

#define TEST_VALUE ((bool)true)

TEST(TestVariableTypeData, BooleanConstructor) {
	VariableTypeData v1(TEST_VALUE);

	if (v1.GetType() != VAR_TYPE_ENUM_BOOL) {
		ASSERT_TRUE(false);
	}
	if (*((bool *) v1.GetData()) != TEST_VALUE) {
		ASSERT_TRUE(false);
	}
}
