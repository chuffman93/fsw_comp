/*! \file testretmsg0011.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 11 of the ReturnMessage Class.
 *
 *	This test verifies that the Flatten method of
 *	Return Message works correctly for a Return Message
 *	with null pointer.
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

#define BUFFER_SIZE 100

TEST(TestReturnMessage, FlattenWithNullPointer) {
	uint8 * buffer = new uint8[BUFFER_SIZE];

	// ReturnMessage with null pointers
	ReturnMessage r1in(NULL, true);

	// Try to flatten with a null buffer
	size_t numCopied = r1in.Flatten(buffer, r1in.GetFlattenSize());

	// Make sure that a null pointer returns 0
	if (numCopied != 0) {
		cout << "Flatten method didn't return 0 when the pointer was null."
				<< endl;

		ASSERT_TRUE(false);
	}

	delete buffer;
}
