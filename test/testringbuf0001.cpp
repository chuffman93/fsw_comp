/*! \file testringbuf0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 1 of the RingBuffer Class.
 *
 *  This test verifies that the constructor correctly initializes the size
 *  of the buffer.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/RingBuffer.h"
using namespace std;
using namespace AllStar::Core;

#define SIZE 1024

TEST(TestStringBuf, Constructor) {
	RingBuffer<unsigned char> rb(SIZE);

	if (rb.free() != SIZE) {
		ASSERT_TRUE(false);
	}
	if (rb.used() != 0) {
		ASSERT_TRUE(false);
	}
}
