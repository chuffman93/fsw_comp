/*! \file testretmsg0010.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 10 of the ReturnMessage Class.
 *
 *	This test verifies that the GetFlattenSize method of
 *	Return Message works correctly
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ReturnMessage.h"
#include "core/DataMessage.h"
#include "core/ErrorMessage.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE1 23
#define TEST_OPCODE2 78
#define TEST_OPCODE3 8
#define TEST_OPCODE4 27

#define TEST_VALUE1 "neato"
#define TEST_VALUE2 true
#define TEST_VALUE3 ((double)12.1)
#define TEST_VALUE4 ((uint32)1234321)

TEST(TestReturnMessage, GetFlattenSize) {
	// ReturnMessage with null pointer
	ReturnMessage r1(NULL, false);

	// Calculate expected size
	size_t expectedSize = 0;

	// Get expected size
	size_t actualSize = r1.GetFlattenSize();

	// Check that sizes are equal
	if (expectedSize != actualSize) {
		cout << "GetFlattenSize() method of ReturnMessage class "
				"did not return correct size." << " GetFlattenSize( ): "
				<< actualSize << " Expected: " << expectedSize << endl;

		ASSERT_TRUE(false);
	}

	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);
	VariableTypeData v3(TEST_VALUE3);
	VariableTypeData v4(TEST_VALUE4);

	list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);
	l1.push_back(&v3);
	l1.push_back(&v4);

	DataMessage d1(TEST_OPCODE1, l1);
	ErrorMessage e1(TEST_OPCODE3, l1);
	ConfigMessage cf1(TEST_OPCODE3, l1);
	CommandMessage cm1(TEST_OPCODE4, l1);

	ReturnMessage r2(&d1, true);
	ReturnMessage r3(&e1, false);
	ReturnMessage r4(&cf1, false);
	ReturnMessage r5(&cm1, true);

	// Calculate expected size
	expectedSize = sizeof(uint8) + sizeof(MessageCodeType) + sizeof(uint16)
			+ sizeof(uint8) + v1.GetFlattenSize() + v2.GetFlattenSize()
			+ v3.GetFlattenSize() + v4.GetFlattenSize();

	// Get actual size
	size_t actualSize2 = r2.GetFlattenSize();
	size_t actualSize3 = r3.GetFlattenSize();
	size_t actualSize4 = r4.GetFlattenSize();
	size_t actualSize5 = r5.GetFlattenSize();

	// Check that sizes are equal
	if (expectedSize != actualSize2 || expectedSize != actualSize3
			|| expectedSize != actualSize4 || expectedSize != actualSize5) {
		cout << "GetFlattenSize() method of ReturnMessage class "
				"did not return correct size." << " GetFlattenSize( ): "
				<< actualSize << " Expected: " << expectedSize << endl;

		ASSERT_TRUE(false);
	}
}
