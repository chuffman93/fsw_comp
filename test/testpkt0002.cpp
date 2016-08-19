/*! \file testvtd0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the ACPPacket Class.
 *
 *  This test verifies that GetFlattenSize works properly
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/ACPPacket.h"

#include "core/ErrorMessage.h"
#include "util/crc.h"

#define LOCATION_ID_MAX 5

using namespace std;
using namespace AllStar::Core;

TEST(TestPacket, GetFlattenSize) {
	MessageCodeType opCode = 6;
	Message* msg = new ErrorMessage(opCode);
	ACPPacket* pkt = new ACPPacket(LOCATION_ID_INVALID, LOCATION_ID_MAX, 42, 0, msg);

	if (pkt->GetSource() != LOCATION_ID_INVALID) {
		ASSERT_TRUE(false);
	}
	if (pkt->GetDestination() != LOCATION_ID_MAX) {
		ASSERT_TRUE(false);
	}
	if (!(*pkt->GetMessagePtr() == msg)) {
		ASSERT_TRUE(false);
	}
	if (pkt->GetNumber() != pkt->GetPacketCounter() - 1) {
		cout << pkt->GetNumber() << " " << pkt->GetPacketCounter() << endl;
		ASSERT_TRUE(false);
	}

	size_t expectedSize = sizeof(LocationIDType) * 2 + sizeof(uint32)
			+ sizeof(uint16) + msg->GetFlattenSize() + sizeof(crc_t);

	if (pkt->GetFlattenSize() != expectedSize) {
		cout << "FlattenSize returns: " << pkt->GetFlattenSize() << endl
				<< "Expected: " << expectedSize << endl;
		ASSERT_TRUE(false);
	}

	delete msg;
	delete pkt;
}
