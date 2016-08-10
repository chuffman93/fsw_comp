/* testarby0003.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 3 of the Arbitrator Class.
 *
 *  This test verifies that the Arbitrator destructor correctly removes all
 *  of the Permissions from the Arbitrator.  This is tested by calling
 *  ClearLocalPermissions, which is the entire body of the destructor.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Arbitrator.h"
using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define SERVER_ID	1

TEST(TestArby, Destructor)
{
	{
		Arbitrator a1(SERVER_ID), * a2;
		Permission p1, p2(true), p3, p4;
		MessageIdentifierType i1(MESSAGE_TYPE_COMMAND, 0), i2(MESSAGE_TYPE_ERROR, 1);

		ASSERT_FALSE(a1.GetServerID() != SERVER_ID);

		a1.ModifyPermission(i1, p1);
		a1.ModifyPermission(i2, p2);

		a2 = new Arbitrator(a1);

		ASSERT_FALSE(a2->GetServerID() != a1.GetServerID());

		ASSERT_EQ(a2->GetPermission(i1, p3), true);

		ASSERT_FALSE(p3 != p1);

		ASSERT_EQ(a2->GetPermission(i2, p4), true);

		ASSERT_FALSE(p4 != p2);

		a2->ClearLocalPermissions();

		ASSERT_EQ(a2->GetPermission(i1, p3), false);

		ASSERT_EQ(a2->GetPermission(i2, p4), false);

		delete a2;
	}
}
