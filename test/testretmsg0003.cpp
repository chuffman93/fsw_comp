/*! \file testretmsg0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 3 of the ReturnMessage Class.
 *
 *	This test verifies that the copy constructor, assignment
 *	operator, and duplicate method all make deep copies of the calling object.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ReturnMessage.h"
#include "core/DataMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE 23
#define TEST_VALUE ((int32)98765)

TEST(TestReturnMessage, CopyAssignmentDuplicate) {
	VariableTypeData v1(TEST_VALUE);

	DataMessage d1(TEST_OPCODE, v1);

	ReturnMessage r1(&d1, false);

	// Copy constructor
	ReturnMessage r2 = r1;

	// Assignment operator
	ReturnMessage r3;
	r3 = r1;

	// Duplicate method
	Message * m1 = r1.Duplicate();

	// Check copy constructor
	if (!(r1 == &r2)) {
		cout << "Copy constructor failed to make deep copy of calling object."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check assignment operator
	if (!(r1 == &r3)) {
		cout
				<< "Assignment operator failed to make deep copy of calling object."
				<< endl;

		ASSERT_TRUE(false);
	}

	// Check duplicate method
	if (!(r1 == m1)) {
		cout << "Duplicate method failed to make deep copy of calling object."
				<< endl;

		ASSERT_TRUE(false);
	}

	delete m1;
}
