/*! \file testvtd0012.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 12 of the VariableTypeData Class.
 *
 *  This test verifies that the assignment operator works correctly for
 *  VAR_TYPE_ENUM_VOID types.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include <vector>

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

TEST(TestVariableTypeData, AssignmentOperatorVoid) {
	VariableTypeData * v1, v2;

	v1 = new VariableTypeData();
	if (v1->GetType() != VAR_TYPE_ENUM_VOID) {
		ASSERT_TRUE(false);
	}

	if (v1->GetData() != NULL) {
		ASSERT_TRUE(false);
	}

	v2 = *v1;

	if (v2.GetType() != VAR_TYPE_ENUM_VOID) {
		ASSERT_TRUE(false);
	}

	if (v2.GetData() != NULL) {
		ASSERT_TRUE(false);
	}

	delete v1;
}
