/* testerrmsg0001.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 1 of the ErrorMessage Class.
 *
 *	This test verifies that constructor with with no
 *	parameters correctly initializes the opcode to 0, and parameters
 *	data to be an empty list.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;


TEST(TestErrMsg, Constructor_NoArgs)
{
	// Default ErrorMessage constructor
	ErrorMessage c1;

	list<VariableTypeData *> l1;

	ErrorMessage c2(0, l1);

	// Check opcode
	ASSERT_TRUE(c1.GetOpcode() == 0);

	// Check parameters
	ASSERT_TRUE(c1.ParametersEqual(c2) == true);
}
