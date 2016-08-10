/* testarby0004.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 4 of the Arbitrator Class.
 * 
 *  This test verifies that the Arbitrator assignment operator correctly sets the
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


TEST(TestArby, Assignment)
{
	{
				Arbitrator a1(SERVER_ID), a2(SERVER_ID+1);
				Permission p1, p2(true), p3, p4;
				MessageIdentifierType i1(MESSAGE_TYPE_COMMAND, 0), i2(MESSAGE_TYPE_ERROR, 1);

				ASSERT_TRUE(a1.GetServerID() == SERVER_ID);


				a1.ModifyPermission(i1, p1);
				a1.ModifyPermission(i2, p2);

				a2 = a1;

				ASSERT_TRUE(a2.GetServerID() == a1.GetServerID());

				ASSERT_EQ(a2.GetPermission(i1, p3), true);


				ASSERT_TRUE(p3 == p1);


				ASSERT_EQ(a2.GetPermission(i2, p4), true);


				ASSERT_TRUE(p4 == p2);


				//a1 = a1;

				ASSERT_TRUE(a1.GetServerID() == SERVER_ID);
				ASSERT_EQ(a1.GetPermission(i1,p3), true);
				ASSERT_EQ(a1.GetPermission(i2,p4), true);

				ASSERT_TRUE(p3 == p1);


				ASSERT_TRUE(p4 == p2);

			}
}
