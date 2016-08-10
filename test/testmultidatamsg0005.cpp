/*! \file testmultdatmsg0005.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 5 of the MultiDataMessage Class.
 *
 *      This test verifies that the Get and Set and Add Parameters
 *      methods of the MultiDataMessage work properly.  Also checks the
 *      parameters equal method by using it as a check.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"
#include "core/Message.h"

using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define TEST_OPCODE1 ((MessageCodeType)58)
#define TEST_VALUE1 ((double)28.9)
#define TEST_VALUE2 "Hello"
#define TEST_VALUE3 true
#define TEST_VALUE4 ((uint32)1234567)
#define TEST_VALUE5 ((int32)7777)

TEST(TestMultiDataMessage, GetSetAddParameters) {
	// Messages that will have their parameters set
	ConfigMessage cf1(TEST_OPCODE1);
	CommandMessage cm1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE1);
	DataMessage d1(TEST_OPCODE1);

	// Data
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	// Data list
	list<VariableTypeData *> l1;

	// Add data to data list
	l1.push_back(&v1);
	l1.push_back(&v2);

	// Set parameters of the messages
	cf1.SetParameters(l1);
	cm1.SetParameters(l1);
	e1.SetParameters(l1);
	d1.SetParameters(l1);

	// Create new messages with the parameters of the now set original messages
	ConfigMessage cf2(TEST_OPCODE1, cf1.GetParameters());
	CommandMessage cm2(TEST_OPCODE1, cm1.GetParameters());
	ErrorMessage e2(TEST_OPCODE1, e1.GetParameters());
	DataMessage d2(TEST_OPCODE1, d1.GetParameters());

	// Check config message
	if (!(cf1.ParametersEqual(cf2))) {
		cout << "Config Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check command message
	if (!(cm1.ParametersEqual(cm2))) {
		cout << "Command Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check error message
	if (!(e1.ParametersEqual(e2))) {
		cout << "Config Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check data message
	if (!(d1.ParametersEqual(d2))) {
		cout << "Data Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}

	// New data list
	list<VariableTypeData *> l2;

	VariableTypeData v3(TEST_VALUE3);
	VariableTypeData v4(TEST_VALUE4);

	// Add new data to new data list
	l2.push_back(&v3);
	l2.push_back(&v4);

	// Set new parameters of messages
	cf1.SetParameters(l2);
	cm1.SetParameters(l2);
	e1.SetParameters(l2);
	d1.SetParameters(l2);

	// Create new messages with the newly set parameters of the original
	ConfigMessage cf3(TEST_OPCODE1, cf1.GetParameters());
	CommandMessage cm3(TEST_OPCODE1, cm1.GetParameters());
	ErrorMessage e3(TEST_OPCODE1, e1.GetParameters());
	DataMessage d3(TEST_OPCODE1, d1.GetParameters());

	// Check config message
	if (!(cf1.ParametersEqual(cf3))) {
		cout << "Config Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check command message
	if (!(cm1.ParametersEqual(cm3))) {
		cout << "Command Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check error message
	if (!(e1.ParametersEqual(e3))) {
		cout << "Config Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check data message
	if (!(d1.ParametersEqual(d3))) {
		cout << "Data Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}

	// New data
	VariableTypeData v5(TEST_VALUE5);

	// Add parameter to messages
	cm1.AddParameter(v5);
	cf1.AddParameter(v5);
	e1.AddParameter(v5);
	d1.AddParameter(v5);

	// Add parameter to lists
	l2.push_back(&v5);

	// Create new messages with updated lists
	ConfigMessage cf4(TEST_OPCODE1, l2);
	CommandMessage cm4(TEST_OPCODE1, l2);
	ErrorMessage e4(TEST_OPCODE1, l2);
	DataMessage d4(TEST_OPCODE1, l2);

	// Check config message
	if (!(cf1.ParametersEqual(cf4))) {
		cout << "Config Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check command message
	if (!(cm1.ParametersEqual(cm4))) {
		cout << "Command Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check error message
	if (!(e1.ParametersEqual(e4))) {
		cout << "Config Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
	// Check data message
	if (!(d1.ParametersEqual(d4))) {
		cout << "Data Message failed to set or get data correctly.";

		ASSERT_TRUE(false);
	}
}
