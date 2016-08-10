/* testarby0005.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 5 of the Arbitrator Class.
 *
 *  This test verifies that ModifyPermission creates a new Permission entry
 *  when there is no permission associated with the given id.  It also
 *  verifies that if there is already a permission for the given id, then
 *  the permission is changed correctly to the new permission.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Arbitrator.h"
using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define SERVER_ID	1

TEST(TestArby, ModifyPermission)
{
	{
		Arbitrator a1(SERVER_ID);
		Permission p1, p2(true), p3;
		MessageIdentifierType i1(MESSAGE_TYPE_COMMAND, 0), i2(MESSAGE_TYPE_ERROR, 1);

		ASSERT_TRUE(a1.GetServerID() == SERVER_ID);


		ASSERT_EQ(a1.GetPermission(i1, p3), false);


		ASSERT_EQ(a1.ModifyPermission(i1, p1), true);


		ASSERT_EQ(a1.GetPermission(i1, p3), true);

		ASSERT_TRUE(p1 == p3);

		ASSERT_EQ(a1.GetPermission(i2, p3), false);


		ASSERT_EQ(a1.ModifyPermission(i2, p2), true);

		ASSERT_EQ(a1.GetPermission(i2, p3), true);

		ASSERT_TRUE(p2 == p3);


		ASSERT_EQ(a1.ModifyPermission(i1, p2), true);


		ASSERT_EQ(a1.GetPermission(i1, p3), true);

		ASSERT_TRUE(p2 == p3);


		ASSERT_EQ(a1.ModifyPermission(i2, p1), true);


		ASSERT_EQ(a1.GetPermission(i2, p3), true);

		ASSERT_TRUE(p1 == p3);

	}
}
