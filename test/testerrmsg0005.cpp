/* testerrmsg0005.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 5 of the ErrorMessage Class.
 *
 *  This test verifies that the copy constructor, assignment operator,
 *  and Duplicate() methods all create a deep copy of the source.
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
#define TEST_VALUE2 false



TEST(TestErrMsg, CC_AO_Duplicate)
{
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);

	ErrorMessage c1(TEST_OPCODE, l1);

	ErrorMessage c2 = c1;

	ErrorMessage c3;
	c3 = c1;

	Message * m1 = c1.Duplicate();


	ASSERT_TRUE(c1 == &c2);

	ASSERT_TRUE(c1 == &c3);

	ASSERT_TRUE(c1 == m1);

	delete m1;
}
