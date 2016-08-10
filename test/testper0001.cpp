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

TEST(TestPermission, DefaultConstructor) {
	//Default constructor no parameter should set all permissions false
	Permission p1;
	//Default constructor with 'true' parameter should set all permissions true
	Permission p2(true);
	//Default constructor with 'false' parameter should set all permissions false
	Permission p3(false);

	size_t i;

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Make sure all permissions of p1 initialized false
		if (p1.ServerHasPermission(i)) {
			cout << "Default permissions constructor failed to initialize all "
					"permissions to false." << endl;
			ASSERT_TRUE(false);
		}

		//Make sure all permissions of p1 initialized true
		if (!p2.ServerHasPermission(i)) {
			cout
					<< "Parameter constructor with boolean parameter equal to true "
							"failed to initialize all permissions to true."
					<< endl;
			ASSERT_TRUE(false);
		}

		//Make sure all permissions of p1 initialized false
		if (p3.ServerHasPermission(i)) {
			cout
					<< "Parameter constructor with boolean parameter equal to false "
							"failed to initialize all permissions to false."
					<< endl;
			ASSERT_TRUE(false);
		}

	}

}
