/* testconfmsg0002.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 2 of the ConfigMessage Class.
 *
 *	This test verifies that constructor with a MessageCodeType parameter correctly
 *  sets the opcode to the parameter's value.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ConfigMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE ((MessageCodeType)34)

TEST(TestConfigMessage, ConstructorMCT)
{
	{
		ConfigMessage c1(TEST_OPCODE);

		list<VariableTypeData *> l1;

		ConfigMessage c2(0, l1);

		ASSERT_TRUE(c1.GetOpcode() == TEST_OPCODE);

		// Check parameters
		ASSERT_EQ((c1.ParametersEqual(c2)), true);
	}
}
