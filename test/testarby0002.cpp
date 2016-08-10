/* testarby0002.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 * \brief  Test 2 of the Arbitrator Class.
 * 
 *  This test verifies that the Arbitrator copy constructor correctly sets the
 *  server ID and copies over the permission list.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Arbitrator.h"
using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define SERVER_ID	1

TEST(TestArby, CopyConst)
{
	{
		Arbitrator a1(SERVER_ID), * a2;
		Permission p1, p2(true), p3, p4;
		MessageIdentifierType i1(MESSAGE_TYPE_COMMAND, 0), i2(MESSAGE_TYPE_ERROR, 1);

		ASSERT_EQ(a1.GetServerID(), SERVER_ID);

		a1.ModifyPermission(i1, p1);
		a1.ModifyPermission(i2, p2);

		a2 = new Arbitrator(a1);

		ASSERT_EQ(a2->GetServerID(), a1.GetServerID());

		ASSERT_EQ(a2->GetPermission(i1, p3), true);

		ASSERT_EQ(p3, p1);

		ASSERT_EQ(a2->GetPermission(i2, p4), true);


		ASSERT_EQ(p4, p2);

		delete a2;
	}
}
