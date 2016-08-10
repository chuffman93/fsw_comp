/*! \file testper0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the Permission Class.
 *
 *  This test verifies that the default constructor with no parameter, and
 *  the default constructor with boolean = false correctly set all server
 *  permissions as false, and the constructor with boolean = true parameter
 *  correctly sets all permissions as true.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Permission.h"

using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

TEST(TestPermission, DefaultAndBoolConstructors)
{
	Permission p1, p2(true), p3(p1), p4(true);

	if (p1 != p3)
	{
		ASSERT_TRUE(false);
	}

	if (p1 == p2)
	{
		ASSERT_TRUE(false);
	}

	if (p2 != p4)
	{
		ASSERT_TRUE(false);
	}
}
