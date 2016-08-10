/* testarby0006.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 6 of the Arbitrator Class.
 *
 *  This test verifies that GetPermission correctly returns false if no
 *  permission exists for the given id.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Arbitrator.h"
using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define SERVER_ID	1


TEST(TestArby, GetPermission)
{
	{
		Arbitrator a1(SERVER_ID);
		Permission p1;
		MessageIdentifierType i1(MESSAGE_TYPE_COMMAND, 0), i2(MESSAGE_TYPE_ERROR, 1);

		ASSERT_EQ(a1.GetPermission(i1, p1), false);

		ASSERT_EQ(a1.GetPermission(i2, p1), false);

	}
}
