/*! \file testretmsg0006.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 6 of the ReturnMessage Class.
 *
 *	This test verifies that the GetType method correctly returns
 *	the type of message stored in the message pointer for any type
 *	of message stored there.
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

TEST(TestReturnMessage, GetType) {
	DataMessage d1;
	ErrorMessage e1;
	ConfigMessage cf1;
	CommandMessage cm1;

	ReturnMessage r1(&d1, true);
	ReturnMessage r2(&e1, false);
	ReturnMessage r3(&cf1, true);
	ReturnMessage r4(&cm1, false);

	// Check type
	if (r1.GetType() != MESSAGE_TYPE_DATA || r2.GetType() != MESSAGE_TYPE_ERROR
			|| r3.GetType() != MESSAGE_TYPE_CONFIG
			|| r4.GetType() != MESSAGE_TYPE_COMMAND) {
		cout << "Get success failed to return the right value." << endl;

		ASSERT_TRUE(false);
	}
}
