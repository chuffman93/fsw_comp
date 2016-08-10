/*! \file testvtd0005.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 5 of the VariableTypeData Class.
 *
 *  This test verifies that the double constructor correctly sets
 *  the data and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

#define TEST_VALUE ((double)15.0)

TEST(TestVariableTypeData, DoubleConstructor) {
	VariableTypeData v1(TEST_VALUE);

	if (v1.GetType() != VAR_TYPE_ENUM_DOUBLE) {
		ASSERT_TRUE(false);
	}
	if (*((double *) v1.GetData()) != TEST_VALUE) {
		ASSERT_TRUE(false);
	}
}
