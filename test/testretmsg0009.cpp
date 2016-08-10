/*! \file testretmsg0009.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 9 of the ReturnMessage Class.
 *
 *	This test verifies that the the comparator works correctly
 *	for return messages
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

#define TEST_CONFIG1 1
#define TEST_CONFIG2 2
#define TEST_CONFIG3 3
#define TEST_CONFIG4 4

TEST(TestReturnMessage, Comparator) {
	ReturnMessage r1(NULL, false);
	ReturnMessage r2(NULL, true);
	ReturnMessage r3(NULL, false);

	// Check return messages with null pointer
	if (r1 == &r2 || !(r1 == &r3)) {
		cout << "ReturnMessage failed to correctly compare two messages."
				<< endl;

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
	DataMessage d2(TEST_OPCODE2, l1);
	ErrorMessage e1(TEST_OPCODE1, l1);
	ErrorMessage e2(TEST_OPCODE2, l1);
	ConfigMessage cf1(TEST_OPCODE1, l1);
	ConfigMessage cf2(TEST_OPCODE2, l1);
	CommandMessage cm1(TEST_OPCODE1, l1);
	CommandMessage cm2(TEST_OPCODE2, l1);

	ReturnMessage rd1(&d1, true);
	ReturnMessage rd2(&d1, false);
	ReturnMessage rd3(&d1, true);
	ReturnMessage rd4(&d2, true);

	ReturnMessage re1(&e1, true);
	ReturnMessage re2(&e1, false);
	ReturnMessage re3(&e1, true);
	ReturnMessage re4(&e2, true);

	ReturnMessage rcf1(&cf1, true);
	ReturnMessage rcf2(&cf1, false);
	ReturnMessage rcf3(&cf1, true);
	ReturnMessage rcf4(&cf2, true);

	ReturnMessage rcm1(&cm1, true);
	ReturnMessage rcm2(&cm1, false);
	ReturnMessage rcm3(&cm1, true);
	ReturnMessage rcm4(&cm2, true);

	if (rd1 == &rd2 || !(rd1 == &rd3) || rd4 == &rd1 || re1 == &re2
			|| !(re1 == &re3) || re4 == &re1 || rcf1 == &rcf2
			|| !(rcf1 == &rcf3) || rcf4 == &rcf1 || rcm1 == &rcm2
			|| !(rcm1 == &rcm3) || rcm4 == &rcm1) {
		cout << "ReturnMessage failed to correctly compare two messages."
				<< endl;

		ASSERT_TRUE(false);
	}
}
