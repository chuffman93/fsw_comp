/*! \file testsendmsg0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 3 of the SendMessage Class.
 *
 *	This test verifies that the copy constructor and
 *	assignment operators both make deep copies of the source.
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

TEST(TestSendMessage, CopyAndAssignment) {
	DataMessage d1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE2);
	ConfigMessage cf1(TEST_OPCODE3);
	CommandMessage cm1(TEST_OPCODE4);

	// Copy constructor
	DataMessage d2 = d1;
	ErrorMessage e2 = e1;
	ConfigMessage cf2 = cf1;
	CommandMessage cm2 = cm1;

	// Assignment operator
	DataMessage d3;
	ErrorMessage e3;
	ConfigMessage cf3;
	CommandMessage cm3;

	d3 = d1;
	e3 = e1;
	cf3 = cf1;
	cm3 = cm1;

	// Check copy constructor
	if (d2.GetOpcode() != d1.GetOpcode() || e2.GetOpcode() != e1.GetOpcode()
			|| cf2.GetOpcode() != cf1.GetOpcode()
			|| cm2.GetOpcode() != cm1.GetOpcode()) {
		cout << "Copy constructor failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check assignment operator
	if (d3.GetOpcode() != d1.GetOpcode() || e3.GetOpcode() != e1.GetOpcode()
			|| cf3.GetOpcode() != cf1.GetOpcode()
			|| cm3.GetOpcode() != cm1.GetOpcode()) {
		cout << "Assignment operator failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}
}
