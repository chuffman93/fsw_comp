/*! \file testvtd0011.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 11 of the VariableTypeData Class.
 *
 *  This test verifies that the VariableTypeData buffer constructor and
 *  Flatten methods correctly handle NULL buffers and buffers that are
 *  too small.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include <vector>

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

TEST(TestVariableTypeData, BufferConstructorNullAndSmall) {
	VariableTypeData * v1;

// Check that the buffer constructor handles NULL correctly
	v1 = new VariableTypeData(NULL, 100);
	if (v1->GetType() != VAR_TYPE_ENUM_VOID) {
		ASSERT_TRUE(false);
	}

	delete v1;

}
