/* testarby0007.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 7 of the Arbitrator Class.
 *
 *  This test verifies that Authenticate returns WRONG_SERVER if the incoming
 *  packet's destination does not match serverID.  It also verifies that
 *  Authenticate returns PERMISSION if the packet is allowed and
 *  NO_PERMISSION if it is not.  Finally, the test verifies that Authenticate
 *  returns NO_PERMISSION if the packet has a message identifier that does
 *  not have a permission within the Arbitrator.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Arbitrator.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define SERVER_ID	1

TEST(TestArby, Authenticate)
{
	{
		Arbitrator a1(SERVER_ID);
		Permission p1, p2(true), p3;
		MessageIdentifierType i1(MESSAGE_TYPE_COMMAND, 0), i2(MESSAGE_TYPE_ERROR, 1);
		CommandMessage * cmd = new CommandMessage(0);
		CommandMessage * cmd2 = new CommandMessage(1);
		ErrorMessage * err = new ErrorMessage(1);
		ACPPacket pkt1(3, 2, 0, 0, cmd), pkt2(2, 1, 1, 0, err);
		ACPPacket pkt3(2, 1, 2, 0, cmd), pkt4(2, 1, 3, 0, cmd2);

		delete cmd;
		delete cmd2;
		delete err;

		ASSERT_EQ(a1.ModifyPermission(i1, p1), true);
	

		ASSERT_EQ(a1.ModifyPermission(i2, p2), true);


		ASSERT_EQ(a1.GetPermission(i1, p3), true);

		ASSERT_TRUE(p1 == p3);

		ASSERT_EQ(a1.GetPermission(i2, p3), true);
		ASSERT_TRUE(p2 == p3);

		ASSERT_TRUE(ARBITRATOR_AUTH_STATUS_WRONG_SERVER == a1.Authenticate(pkt1));


		ASSERT_TRUE(ARBITRATOR_AUTH_STATUS_PERMISSION == a1.Authenticate(pkt2));


		ASSERT_TRUE(ARBITRATOR_AUTH_STATUS_NO_PERMISSION == a1.Authenticate(pkt3));


		ASSERT_TRUE(ARBITRATOR_AUTH_STATUS_NO_PERMISSION == a1.Authenticate(pkt4));

	}
}
