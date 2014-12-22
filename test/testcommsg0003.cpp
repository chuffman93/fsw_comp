/* testcommsg0003.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 3 of the CommandMessage Class.
 *
 *  This test verifies that constructor with a MessageCodeType, and single VariableTypeData
 *  parameter correctly sets the opcode and data to the parameters' values.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;

#define TEST_OPCODE ((MessageCodeType)34)
#define TEST_VALUE1 ((double)39.7)


TEST(TestCommandMessage, ConstructorMCTVTD)
{
	{
		VariableTypeData v1(TEST_VALUE1);

		CommandMessage c1(TEST_OPCODE, v1);
		CommandMessage c2(TEST_OPCODE, v1);

		ASSERT_TRUE(c1.GetOpcode() == TEST_OPCODE);


		ASSERT_EQ((c1.ParametersEqual(c2)), true);

	}
}
