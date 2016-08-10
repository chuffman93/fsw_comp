/*! \file testsendmsg0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the SendMessage Class.
 *
 *	This test verifies that the IsResponse and GetSuccess
 *	methods return false for all SendMessages with both constructors
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

TEST(TestSendMessage, IsResponseAndGetSuccess) {
	DataMessage d1;
	ErrorMessage e1;
	ConfigMessage cf1;
	CommandMessage cm1;

	DataMessage d2(1);
	ErrorMessage e2(1);
	ConfigMessage cf2(1);
	CommandMessage cm2(1);

	if (d1.IsResponse() || e1.IsResponse() || cf1.IsResponse()
			|| cm1.IsResponse() || d2.IsResponse() || e2.IsResponse()
			|| cf2.IsResponse() || cm2.IsResponse()) {
		cout << "SendMessage did not return false for IsResponse" << endl;

		ASSERT_TRUE(false);
	}

	if (d1.GetSuccess() || e1.GetSuccess() || cf1.GetSuccess()
			|| cm1.GetSuccess() || d2.GetSuccess() || e2.GetSuccess()
			|| cf2.GetSuccess() || cm2.GetSuccess()) {
		cout << "SendMessage did not return false for IsResponse" << endl;

		ASSERT_TRUE(false);
	}
}
