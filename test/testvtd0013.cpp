/*! \file testvtd0010.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 13 of the VariableTypeData Class.
 *
 *  This test verifies that the packet constructor correctly sets
 *  the data and type fields.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
#include "core/ACPPacket.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

TEST(TestVariableTypeData, PacketConstructor) {
	CommandMessage cmd(1);
	ACPPacket packet(1, 2, 0, 0, &cmd);
	VariableTypeData v1(packet);

	if (v1.GetType() != VAR_TYPE_ENUM_PACKET) {
		ASSERT_TRUE(false);
	}

	if (!(*((ACPPacket *) v1.GetData()) == packet)) {
		ASSERT_TRUE(false);
	}
}
