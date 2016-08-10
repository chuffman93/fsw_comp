/* testerrmsg0003.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 3 of the ErrorMessage Class.
 *
 *  This test verifies that constructor with a MessageCodeType, and single VariableTypeError
 *  parameter correctly sets the opcode and data to the parameters' values.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

#define TEST_OPCODE ((MessageCodeType)34)
#define TEST_VALUE1 ((double)39.7)


TEST(TestErrMsg, Constructor_MCT_VTE)
{
	VariableTypeData v1(TEST_VALUE1);

	ErrorMessage c1(TEST_OPCODE, v1);
	ErrorMessage c2(TEST_OPCODE, v1);

	ASSERT_TRUE(c1.GetOpcode() == TEST_OPCODE);

	ASSERT_TRUE(c1.ParametersEqual(c2) == true);
}
