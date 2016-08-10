/*! \file testretmsg0008.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 8 of the ReturnMessage Class.
 *
 *	This test verifies that the GetMessagePtr correctly returns
 *	the message pointer held by the class.
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

TEST(TestReturnMessage, GetMessagePointer) {
	DataMessage d1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE2);
	ConfigMessage cf1(TEST_OPCODE3);
	CommandMessage cm1(TEST_OPCODE4);

	ReturnMessage r1(&d1, true);
	ReturnMessage r2(&e1, false);
	ReturnMessage r3(&cf1, true);
	ReturnMessage r4(&cm1, false);

	// Check opcode
	if (!(d1 == r1.GetMessagePtr()) || !(e1 == r2.GetMessagePtr())
			|| !(cf1 == r3.GetMessagePtr()) || !(cm1 == r4.GetMessagePtr())) {
		cout
				<< "Get message pointer failed to return the right message pointer."
				<< endl;

		ASSERT_TRUE(false);
	}
}
