/*! \file testmultidatamsg0004.cpp
 *  * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 4 of the MultiDataMessage Class.
 *
 *  This test verifies that the copy constructor and
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

#define TEST_VALUE1 ((double)28.9)
#define TEST_VALUE2 "Hello"

TEST(TestMultiDataMessage, CopyConstructor) {
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	std::list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);

	DataMessage d1(TEST_OPCODE1, l1);
	ErrorMessage e1(TEST_OPCODE2, l1);
	ConfigMessage cf1(TEST_OPCODE3, l1);
	CommandMessage cm1(TEST_OPCODE4, l1);

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

	// Check opcodes
	if (d1.GetOpcode() != d2.GetOpcode() || e1.GetOpcode() != e2.GetOpcode()
			|| cf1.GetOpcode() != cf2.GetOpcode()
			|| cm1.GetOpcode() != cm2.GetOpcode()) {
		cout << "Copy constructor failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check opcodes
	if (d1.GetOpcode() != d3.GetOpcode() || e1.GetOpcode() != e3.GetOpcode()
			|| cf1.GetOpcode() != cf3.GetOpcode()
			|| cm1.GetOpcode() != cm3.GetOpcode()) {
		cout << "Assignment operator failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check parameter list size is correct
	if (d1.GetParameters().size() != d2.GetParameters().size()
			|| e1.GetParameters().size() != e2.GetParameters().size()
			|| cf1.GetParameters().size() != cf2.GetParameters().size()
			|| cm1.GetParameters().size() != cm2.GetParameters().size()) {
		cout << "Copy constructor failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check parameter list size is correct
	if (d1.GetParameters().size() != d3.GetParameters().size()
			|| e1.GetParameters().size() != e3.GetParameters().size()
			|| cf1.GetParameters().size() != cf3.GetParameters().size()
			|| cm1.GetParameters().size() != cm3.GetParameters().size()) {
		cout << "Assignment operator failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	std::list<VariableTypeData*> d1l = d1.GetParameters();
	std::list<VariableTypeData*> e1l = e1.GetParameters();
	std::list<VariableTypeData*> cf1l = cf1.GetParameters();
	std::list<VariableTypeData*> cm1l = cm1.GetParameters();

	std::list<VariableTypeData*> d2l = d2.GetParameters();
	std::list<VariableTypeData*> e2l = e2.GetParameters();
	std::list<VariableTypeData*> cf2l = cf2.GetParameters();
	std::list<VariableTypeData*> cm2l = cm2.GetParameters();

	std::list<VariableTypeData*> d3l = d3.GetParameters();
	std::list<VariableTypeData*> e3l = e3.GetParameters();
	std::list<VariableTypeData*> cf3l = cf3.GetParameters();
	std::list<VariableTypeData*> cm3l = cm3.GetParameters();

	// Check parameter value is correct
	if (*(d1l.front()) != *(d2l.front()) || *(e1l.front()) != *(e2l.front())
			|| *(cf1l.front()) != *(cf2l.front())
			|| *(cm1l.front()) != *(cm2l.front())) {
		cout << "Copy constructor failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check parameter value is correct
	if (*(d1l.front()) != *(d3l.front()) || *(e1l.front()) != *(e3l.front())
			|| *(cf1l.front()) != *(cf3l.front())
			|| *(cm1l.front()) != *(cm3l.front())) {
		cout << "Assignment operator failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	d1l.pop_front();
	e1l.pop_front();
	cf1l.pop_front();
	cm1l.pop_front();

	d2l.pop_front();
	e2l.pop_front();
	cf2l.pop_front();
	cm2l.pop_front();

	d3l.pop_front();
	e3l.pop_front();
	cf3l.pop_front();
	cm3l.pop_front();

	// Check parameter value is correct
	if (*(d1l.front()) != *(d2l.front()) || *(e1l.front()) != *(e2l.front())
			|| *(cf1l.front()) != *(cf2l.front())
			|| *(cm1l.front()) != *(cm2l.front())) {
		cout << "Copy constructor failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check parameter value is correct
	if (*(d1l.front()) != *(d3l.front()) || *(e1l.front()) != *(e3l.front())
			|| *(cf1l.front()) != *(cf3l.front())
			|| *(cm1l.front()) != *(cm3l.front())) {
		cout << "Assignment operator failed to make a deep copy of source."
				<< endl;

		ASSERT_TRUE(false);
	}
}
