/*! \file testvtd0010.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 10 of the VariableTypeData Class.
 *
 *  This test verifies that the array constructor correctly sets
 *  the data and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include <vector>

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

TEST(TestVariableTypeData, ArrayConstructor) {
	vector<uint8> vec;
	vec.push_back(10);
	vec.push_back(12);
	vec.push_back(5);
	VariableTypeData v1(vec);

	if (v1.GetType() != VAR_TYPE_ENUM_ARRAY) {
		ASSERT_TRUE(false);
	}

	if (!(*((vector<uint8> *) v1.GetData()) == vec)) {
		ASSERT_TRUE(false);
	}
}
