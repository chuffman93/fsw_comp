/*! \file testvtd0006.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 6 of the VariableTypeData Class.
 *
 *  This test verifies that the float constructor correctly sets
 *  the data and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

#define TEST_VALUE ((float)15.0)

TEST(TestVariableTypeData, FloatConstructor) {
	VariableTypeData v1(TEST_VALUE);

	if (v1.GetType() != VAR_TYPE_ENUM_FLOAT) {
		ASSERT_TRUE(false);
	}
	if (*((float *) v1.GetData()) != TEST_VALUE) {
		ASSERT_TRUE(false);
	}
}
