/*! \file testmultdatmsg0008.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 8 of the MultiDataMessage Class.
 *
 *      This test verifies that the GetFlattenSize Method of the
 *      MultiDatMessage class works correctly.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE1 ((MessageCodeType)76)
#define TEST_OPCODE2 ((MessageCodeType)25)
#define TEST_VALUE1 ((double)27.2)
#define TEST_VALUE2 "Hello"

TEST(TestMultiDataMessage, GetFlattenSize) {
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);

	ConfigMessage cf(TEST_OPCODE1, l1);
	CommandMessage cm(TEST_OPCODE2, l1);
	DataMessage d(TEST_OPCODE2, l1);
	ErrorMessage e(TEST_OPCODE2, l1);

	// Calculate expected size
	size_t expectedSize = sizeof(uint8) + sizeof(MessageCodeType)
			+ sizeof(uint16) + sizeof(uint8) + v1.GetFlattenSize()
			+ v2.GetFlattenSize();

	// Get actual size
	size_t cfActualSize = cf.GetFlattenSize();
	size_t cmActualSize = cm.GetFlattenSize();
	size_t dActualSize = d.GetFlattenSize();
	size_t eActualSize = e.GetFlattenSize();

	// Check that sizes are the same
	if (expectedSize != cfActualSize || expectedSize != cmActualSize
			|| expectedSize != dActualSize || expectedSize != eActualSize) {
		cout << "GetFlattenSize() method of MultiDataMessage class "
				"did not return correct size." << " GetFlattenSize( ): {CF: "
				<< cfActualSize << ", CM" << cmActualSize << ", D"
				<< dActualSize << ", E" << eActualSize << "\n" << " Expected: "
				<< expectedSize << endl;

		ASSERT_TRUE(false);
	}
}
