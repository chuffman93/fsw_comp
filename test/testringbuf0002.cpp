/*! \file testringbuf0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 2 of the RingBuffer Class.
 *
 *  This test verifies the correctness of the read, write and clear methods.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include <cstring>

#include "core/RingBuffer.h"
using namespace std;
using namespace AllStar::Core;

#define SIZE 64

TEST(TestStringBuf, ReadWriteClear) {
	RingBuffer<char> rb(SIZE);
	static const char text1[23] = "this is a short string";
	static const char text2[59] =
			"this is a long string which will cause the buffer to wrap.";
	char buf[512];

	// short string, no wrap
	if (rb.write(text1, 23) != 23) {
		ASSERT_TRUE(false);
	}
	if (rb.used() != 23) {
		ASSERT_TRUE(false);
	}
	if (rb.read(buf, 23) != 23) {
		ASSERT_TRUE(false);
	}
	if (strncmp(text1, buf, 23) != 0) {
		ASSERT_TRUE(false);
	}
	if (rb.used() != 0) {
		ASSERT_TRUE(false);
	}
	// long string, wrap
	if (rb.write(text2, 59) != 59) {
		ASSERT_TRUE(false);
	}
	if (rb.used() != 59) {
		ASSERT_TRUE(false);
	}
	if (rb.read(buf, 59) != 59) {
		ASSERT_TRUE(false);
	}
	if (strncmp(text2, buf, 59) != 0) {
		ASSERT_TRUE(false);
	}
	if (rb.used() != 0) {
		ASSERT_TRUE(false);
	}
	// clear
	if (rb.write(text2, 59) != 59) {
		ASSERT_TRUE(false);
	}
	rb.clear();
	if (rb.used() != 0) {
		ASSERT_TRUE(false);
	}
	if (rb.free() != SIZE) {
		ASSERT_TRUE(false);
	}
}
