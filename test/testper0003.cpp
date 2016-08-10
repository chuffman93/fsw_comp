/*! \file testper0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 3 of the Permission Class.
 *
 *	This test verifies that the ServerHasPermission method correctly returns
 *	whether or not a given server has permission, and that the SetServerPermission
 *	correctly sets the permission of a given server.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Permission.h"
using namespace std;
using namespace AllStar::Core;

TEST(TestPermission, ServerHasPermission) {
	//Default constructor no parameter should set all permissions false
	Permission p1;
	//Default constructor with 'true' parameter should set all permissions true
	Permission p2(true);

	size_t i;

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Make sure all permissions of p1 initialized false
		if (p1.ServerHasPermission(i)) {
			cout << "Default permissions constructor failed to initialize all "
					"permissions to false" << endl;

			ASSERT_TRUE(false);
		}

		//Make sure all permissions of p2 initialized true
		if (!p2.ServerHasPermission(i)) {
			cout << "Default permissions constructor failed to initialize all "
					"permissions to false" << endl;

			ASSERT_TRUE(false);
		}

		//Set every permission evenly divisible by 3 or 2, to true
		if (i % 3 == 0 || i % 2 == 0) {
			p1.SetServerPermission(i, true);
			p2.SetServerPermission(i, true);
		}
		//Set all other permissions to false
		else {
			p1.SetServerPermission(i, false);
			p2.SetServerPermission(i, false);
		}
	}

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Check every permission evenly divisible by 3 or 2
		if (i % 3 == 0 || i % 2 == 0) {
			//Check that the permission in p1 was correctly set and returned
			if (!p1.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}

			//Check that the permission in p2 was correctly set and returned
			if (!p2.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
		}
		//Check every other permission
		else {
			//Check that the permission in p1 was correctly set and returned
			if (p1.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
			//Check that the permission in p2 was correctly set and returned
			if (p2.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
		}
	}

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Set every permission evenly divisible by 4, to true
		if (i % 4 == 0) {
			p1.SetServerPermission(i, true);
			p2.SetServerPermission(i, true);
		}
		//Set all other permissions to false
		else {
			p1.SetServerPermission(i, false);
			p2.SetServerPermission(i, false);
		}
	}

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Check every permission evenly divisible by 4
		if (i % 4 == 0) {
			//Check that the permission in p1 was correctly set and returned
			if (!p1.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
			//Check that the permission in p2 was correctly set and returned
			if (!p2.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
		}
		//Check every other permission
		else {
			//Check that the permission in p1 was correctly set and returned
			if (p1.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
			//Check that the permission in p2 was correctly set and returned
			if (p2.ServerHasPermission(i)) {
				cout
						<< "Permissions object did not correctly set permission of a"
								"given server, or did not correctly return that permission"
						<< endl;

				ASSERT_TRUE(false);
			}
		}
	}
}
