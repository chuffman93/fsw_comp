/*! \file testmultidatamsg0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the MultiDataMessage Class.
 *
 *	This test verifies that the GetOpcode method works
 *	for all MultiDataMessages, and the default constructor correctly initializes
 *	the opcode to 0 and an empty parameter list, and the opcode parameter constructor initializes the value
 *	the correctly, also with an empty parameter list.
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

TEST(TestMultiDataMessage, GetOpCode)
{
	DataMessage d1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE2);
	ConfigMessage cf1(TEST_OPCODE3);
	CommandMessage cm1(TEST_OPCODE4);

	if (d1.GetOpcode() != TEST_OPCODE1 || e1.GetOpcode() != TEST_OPCODE2 ||
		cf1.GetOpcode() != TEST_OPCODE3 || cm1.GetOpcode() != TEST_OPCODE4)
	{
		cout << "MultiDataMessage did not return the correct opcode, or the constructor failed"
				"to initialize the value correctly." << endl;

		ASSERT_TRUE(false);
	}

	if (d1.GetParameters().size() != 0 || e1.GetParameters().size() != 0 ||
		cf1.GetParameters().size() != 0 || cm1.GetParameters().size() != 0)
	{
		cout << "MultiDataMessage did not return the correct parameters, or the constructor failed"
				"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}

	DataMessage d2;
	ErrorMessage e2;
	ConfigMessage cf2;
	CommandMessage cm2;

	if (d2.GetOpcode() != 0 || e2.GetOpcode() != 0 ||
		cf2.GetOpcode() != 0 || cm2.GetOpcode() != 0)
	{
		cout << "SendMessage did not return the correct opcode, or the default constructor"
				"failed to initialize the value to 0." << endl;

		ASSERT_TRUE(false);
	}

	if (d2.GetParameters().size() != 0 || e2.GetParameters().size() != 0 ||
		cf2.GetParameters().size() != 0 || cm2.GetParameters().size() != 0)
	{
		cout << "MultiDataMessage did not return the correct parameters, or the constructor failed"
				"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}
}
