/* testarby0001.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 * \brief Test 1 of the Arbitrator Class.
 * 
 *  This test verifies that the Arbitrator constructor correctly sets the
 *  server ID.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Arbitrator.h"
using namespace std;
using namespace AllStar::Core;

#define SERVER_ID	1



TEST(TestArby, Constructor)
{
	{
		Arbitrator a1(SERVER_ID);

		ASSERT_TRUE(a1.GetServerID() == SERVER_ID);

	}
}
