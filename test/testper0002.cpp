/*! \file testper0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the Permission Class.
 *
 *  This test verifies that the copy and assignment constructors correctly
 *  makes the calling Permission object a deep copy of the source object.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Permission.h"

using namespace std;
using namespace AllStar::Core;

TEST(TestPermission, CopyAndAssignmentConstructors) {
	//Default constructor no parameter should set all permissions false
	Permission p1;

	size_t i;

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Make sure all permissions of p1 initialized false
		if (p1.ServerHasPermission(i) != 0) {
			cout << "Default permissions constructor failed to initialize all "
					"permissions to false" << endl;

			ASSERT_TRUE(false);
		}

		//Set every third permission to be true
		if (i % 3 == 0) {
			p1.SetServerPermission(i, 1);
		}

	}

	//Copy constructor
	Permission p2 = p1;

	//Assignment operator
	Permission p3;
	p3 = p1;

	for (i = 0; i < ALLSTAR_NUM_SERVERS; i++) {
		//Check every third permission
		if (i % 3 == 0) {
			//Check that copy constructor initialized permissions to be same as source
			if (p2.ServerHasPermission(i) != 1) {
				cout
						<< "The copy constructor failed to make the calling permission object"
								"a deep copy of the source.";
				ASSERT_TRUE(false);
			}
			//Check that assignment operator initialized permissions to be same as source
			if (p3.ServerHasPermission(i) != 1) {
				cout
						<< "The assignment constructor failed to make the calling permission object"
								"a deep copy of the source.";
				ASSERT_TRUE(false);
			}
		}
		//Check every other permission
		else {
			//Check that copy constructor initialized permissions to be same as source
			if (p2.ServerHasPermission(i) != 0) {
				cout
						<< "The copy constructor failed to make the calling permission object"
								"a deep copy of the source.";
				ASSERT_TRUE(false);
			}
			//Check that assignment operator initialized permissions to be same as source
			if (p3.ServerHasPermission(i) != 0) {
				cout
						<< "The assignment constructor failed to make the calling permission object"
								"a deep copy of the source.";
				ASSERT_TRUE(false);
			}

		}

	}
}
