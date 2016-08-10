/*! \file testmultidatamsg0002.cpp
 *  * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the MultiDataMessage Class.
 *
 *  This test verifies that the single VTD parameter constructor works
 *  properly, initializing the opcode to the correct value, and also the
 *  parameter list to contain the single parameter.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

#define TEST_OPCODE1 23
#define TEST_OPCODE2 43
#define TEST_OPCODE3 62
#define TEST_OPCODE4 87

#define TEST_VALUE1 ((double)28.9)
#define TEST_VALUE2 "Hello"
#define TEST_VALUE3 true
#define TEST_VALUE4 ((uint32)1234567)

TEST(TestMultiDataMessage, VTDConstructor) {
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);
	VariableTypeData v3(TEST_VALUE3);
	VariableTypeData v4(TEST_VALUE4);

	DataMessage d1(TEST_OPCODE1, v1);
	ErrorMessage e1(TEST_OPCODE2, v2);
	ConfigMessage cf1(TEST_OPCODE3, v3);
	CommandMessage cm1(TEST_OPCODE4, v4);

	// Check opcodes
	if (d1.GetOpcode() != TEST_OPCODE1 || e1.GetOpcode() != TEST_OPCODE2
			|| cf1.GetOpcode() != TEST_OPCODE3
			|| cm1.GetOpcode() != TEST_OPCODE4) {
		cout
				<< "MultiDataMessage did not return the correct opcode, or the constructor failed"
						"to initialize the value correctly." << endl;

		ASSERT_TRUE(false);
	}

	// Check only one parameter exists
	if (d1.GetParameters().size() != 1 || e1.GetParameters().size() != 1
			|| cf1.GetParameters().size() != 1
			|| cm1.GetParameters().size() != 1) {
		cout
				<< "MultiDataMessage did not return the correct parameters (size), or the constructor failed"
						"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}

	// Check parameter value is correct
	if (*(d1.GetParameters().front()) != v1
			|| *(e1.GetParameters().front()) != v2
			|| *(cf1.GetParameters().front()) != v3
			|| *(cm1.GetParameters().front()) != v4) {
		cout
				<< "MultiDataMessage did not return the correct parameters (value), or the constructor failed"
						"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}

	DataMessage d2;
	ErrorMessage e2;
	ConfigMessage cf2;
	CommandMessage cm2;

	if (d2.GetOpcode() != 0 || e2.GetOpcode() != 0 || cf2.GetOpcode() != 0
			|| cm2.GetOpcode() != 0) {
		cout
				<< "SendMessage did not return the correct opcode, or the default constructor"
						"failed to initialize the value to 0." << endl;

		ASSERT_TRUE(false);
	}

	if (d2.GetParameters().size() != 0 || e2.GetParameters().size() != 0
			|| cf2.GetParameters().size() != 0
			|| cm2.GetParameters().size() != 0) {
		cout
				<< "MultiDataMessage did not return the correct parameters (size), or the constructor failed"
						"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}
}
