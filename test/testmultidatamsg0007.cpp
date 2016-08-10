/*! \file testmultdatmsg0007.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 7 of the MultiDataMessage Class.
 *
 *      This test verifies that == operator of the MultiDataMessage
 *      correctly compares two MultiDataMessages.
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
#define TEST_VALUE2 true
#define TEST_VALUE3 ((int32)765)
#define TEST_VALUE4 "sweet"

TEST(TestMultiDataMessage, EqualsEquals) {
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);
	VariableTypeData v3(TEST_VALUE3);
	VariableTypeData v4(TEST_VALUE4);

	list<VariableTypeData *> l3;
	list<VariableTypeData *> l4;

	l3.push_back(&v1);
	l3.push_back(&v2);

	l4.push_back(&v3);
	l4.push_back(&v4);

	ConfigMessage cf1(TEST_OPCODE1, l3);
	ConfigMessage cf2(TEST_OPCODE1, l3);
	ConfigMessage cf3(TEST_OPCODE2, l3);
	ConfigMessage cf4(TEST_OPCODE1, l4);
	ConfigMessage cf5(TEST_OPCODE2, l4);

	CommandMessage cm1(TEST_OPCODE1, l3);
	CommandMessage cm2(TEST_OPCODE1, l3);
	CommandMessage cm3(TEST_OPCODE2, l3);
	CommandMessage cm4(TEST_OPCODE1, l4);
	CommandMessage cm5(TEST_OPCODE2, l4);

	DataMessage d1(TEST_OPCODE1, l3);
	DataMessage d2(TEST_OPCODE1, l3);
	DataMessage d3(TEST_OPCODE2, l3);
	DataMessage d4(TEST_OPCODE1, l4);
	DataMessage d5(TEST_OPCODE2, l4);

	ErrorMessage e1(TEST_OPCODE1, l3);
	ErrorMessage e2(TEST_OPCODE1, l3);
	ErrorMessage e3(TEST_OPCODE2, l3);
	ErrorMessage e4(TEST_OPCODE1, l4);
	ErrorMessage e5(TEST_OPCODE2, l4);

	// Check messages with opcode and VariableTypeData list parameters
	if (!(cf1 == &cf2) || (cf1 == &cf3) || (cf1 == &cf4) || (cf1 == &cf5)
			|| !(cm1 == &cm2) || (cm1 == &cm3) || (cm1 == &cm4) || (cm1 == &cm5)
			|| !(d1 == &d2) || (d1 == &d3) || (d1 == &d4) || (d1 == &d5)
			|| !(e1 == &e2) || (e1 == &e3) || (e1 == &e4) || (e1 == &e5)) {
		cout << "The == operator failed to correctly compare two "
				"MultiDataMessages." << endl;
		ASSERT_TRUE(false);
	}

	// Check for types
	if (cm1 == &cf1 || cm1 == &d1 || cm1 == &e1 || cf1 == &d1 || cf1 == &e1
			|| d1 == &e1) {
		cout << "The == operator failed to correctly compare two "
				"SingleDataMessages." << endl;
		ASSERT_TRUE(false);
	}
}
