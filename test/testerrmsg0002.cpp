/* testerrmsg0002.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 1 of the ErrorMessage Class.
 *
 *	This test verifies that constructor with a MessageCodeType parameter correctly
 *  sets the opcode to the parameter's value.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE ((MessageCodeType)34)

TEST(TestErrMsg, Constructor_MCT)
{
	ErrorMessage c1(TEST_OPCODE);

	list<VariableTypeData *> l1;

	ErrorMessage c2(0, l1);

	ASSERT_TRUE(c1.GetOpcode() == TEST_OPCODE);

	// Check parameters
	ASSERT_TRUE(c1.ParametersEqual(c2) == true);
}
