/*! \file testvtd0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the VariableTypeData Class.
 *
 *  This test verifies that the default constructor correctly sets the type
 *  of the data to Void and the pointer to NULL.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

TEST(TestVariableTypeData, DefaultConstructor) {
	VariableTypeData v1;

	if (v1.GetType() != VAR_TYPE_ENUM_VOID) {
		ASSERT_TRUE(false);
	}
	if (v1.GetData() != NULL) {
		ASSERT_TRUE(false);
	}
}
