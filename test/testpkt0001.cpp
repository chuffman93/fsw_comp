/*! \file testvtd0001.cpp
 *  * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the FSWPacket Class.
 *
 *  This test verifies that the default constructor correctly sets the type
 *  of the data to Void and the pointer to NULL.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/FSWPacket.h"

using namespace std;
using namespace Phoenix::Core;

#define LOCATION_ID_MAX 5

TEST(TestPacket, DefaultConstructor) {
	cout << "Starting FSWPacket Test" << endl;

	FSWPacket* pkt = new FSWPacket();

	if (pkt->GetSource() != LOCATION_ID_INVALID) {
		ASSERT_TRUE(false);
	}
	if (pkt->GetDestination() != LOCATION_ID_INVALID) {
		ASSERT_TRUE(false);
	}
	if (pkt->GetMessagePtr() != NULL) {
		ASSERT_TRUE(false);
	}
	pkt->SetNumber(32);
	if (pkt->GetNumber() != 32) {
		ASSERT_TRUE(false);
	}
	pkt->SetDestination( LOCATION_ID_MAX);
	if (pkt->GetDestination() != LOCATION_ID_MAX) {
		ASSERT_TRUE(false);
	}

	delete pkt;
}
