/*! \file testvtd0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the VariableTypeData Class.
 *
 *  This test verifies that the integer constructor correctly sets the data
 *  and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

#define TEST_VALUE ((int32)5)

TEST(TestVariableTypeData, IntegerConstructor) {
	VariableTypeData v1(TEST_VALUE);

	if (v1.GetType() != VAR_TYPE_ENUM_INT) {
		ASSERT_TRUE(false);
	}
	if (*((int32 *) v1.GetData()) != TEST_VALUE) {
		ASSERT_TRUE(false);
	}
}
