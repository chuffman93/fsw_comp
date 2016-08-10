/* testcommsg0001.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 1 of the CommandMessage Class.
 *
 *	This test verifies that constructor with with no
 *	parameters correctly initializes the opcode to 0, and parameters
 *	data to be an empty list.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

TEST(TestCommandMessage, ConstructorEmpty)
{
	// Default CommandMessage constructor
	CommandMessage c1;

	list<VariableTypeData *> l1;

	CommandMessage c2(0, l1);

	// Check opcode
	ASSERT_TRUE(c1.GetOpcode() == 0);


	// Check parameters
	ASSERT_EQ(c1.ParametersEqual(c2), true);

}
