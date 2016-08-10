/*! \file testsendmsg0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the SendMessage Class.
 *
 *	This test verifies that the GetOpcode method works
 *	for all SendMessages, and the default constructor correctly initializes
 *	the opcode to 0, and the opcode parameter constructor initializes the value
 *	the correctly.
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

TEST(TestSendMessage, GetOpCode) {
	DataMessage d1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE2);
	ConfigMessage cf1(TEST_OPCODE3);
	CommandMessage cm1(TEST_OPCODE4);

	if (d1.GetOpcode() != TEST_OPCODE1 || e1.GetOpcode() != TEST_OPCODE2
			|| cf1.GetOpcode() != TEST_OPCODE3
			|| cm1.GetOpcode() != TEST_OPCODE4) {
		cout
				<< "SendMessage did not return the correct opcode, or the constructor failed"
						"to initialize the value correctly." << endl;

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
}
