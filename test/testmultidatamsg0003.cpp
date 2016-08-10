/*! \file testmultidatamsg0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 3 of the MultiDataMessage Class.
 *
 *  This test verifies that the VTD list parameter constructor works
 *  properly, initializing the opcode to the correct value, and also the
 *  parameter list to contain the list of parameters.
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


TEST(TestMultiDataMessage, VTDListConstructor)
{
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	std::list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);

	DataMessage d1(TEST_OPCODE1, l1);
	ErrorMessage e1(TEST_OPCODE2, l1);
	ConfigMessage cf1(TEST_OPCODE3, l1);
	CommandMessage cm1(TEST_OPCODE4, l1);

	// Check opcodes
	if (d1.GetOpcode() != TEST_OPCODE1 || e1.GetOpcode() != TEST_OPCODE2 ||
		cf1.GetOpcode() != TEST_OPCODE3 || cm1.GetOpcode() != TEST_OPCODE4)
	{
		cout << "MultiDataMessage did not return the correct opcode, or the constructor failed"
				"to initialize the value correctly." << endl;

		ASSERT_TRUE(false);
	}

	// Check only parameter list size is correct
	if (d1.GetParameters().size() != 2 || e1.GetParameters().size() != 2 ||
		cf1.GetParameters().size() != 2 || cm1.GetParameters().size() != 2)
	{
		cout << "MultiDataMessage did not return the correct parameters, or the constructor failed"
				"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}

	std::list<VariableTypeData *> d1l = d1.GetParameters();
	std::list<VariableTypeData *> e1l = e1.GetParameters();
	std::list<VariableTypeData *> cf1l = cf1.GetParameters();
	std::list<VariableTypeData *> cm1l = cm1.GetParameters();


	// Check parameter value is correct
	if (*(d1l.front()) != v1 || *(e1l.front()) != v1 ||
		*(cf1l.front()) != v1 || *(cm1l.front()) != v1)
	{
		cout << "MultiDataMessage did not return the correct parameters (value), or the constructor failed"
				"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}

	d1l.pop_front();
	e1l.pop_front();
	cf1l.pop_front();
	cm1l.pop_front();

	// Check parameter value is correct
	if (*(d1l.front()) != v2 || *(e1l.front()) != v2 ||
		*(cf1l.front()) != v2 || *(cm1l.front()) != v2)
	{
		cout << "MultiDataMessage did not return the correct parameters (value), or the constructor failed"
				"to initialize their value correctly." << endl;

		ASSERT_TRUE(false);
	}
}
