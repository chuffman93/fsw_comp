#include "gtest/gtest.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "core/StdTypes.h"
#include "core/ACPPacket.h"
#include "servers/DispatchStdTasks.h"
#include "util/FileHandler.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;
using namespace AllStar::Servers;

TEST(TestACPPacket2, testUInt){
	uint8 testUInt[] = {0x01, 0x23, 0x45, 0x67};
	uint32 result = GetUInt(testUInt);

	ASSERT_EQ(0x01234567, result);
}

TEST(TestACPPacket2, testInt){
	uint8 testInt[] = {0x34, 0x56, 0x78, 0x9A};
	int32 result = GetInt(testInt);

	ASSERT_EQ(0x3456789A, result);
}

TEST(TestACPPacket2, testBool){
	uint8 testBool[] = {0x00};
	bool result = GetBool(testBool);

	ASSERT_EQ(false , result);
}

TEST(TestACPPacket2, testFloat){
	uint8 testFloat[] = {0x34, 0x56, 0x78, 0x9A};
	float result = GetFloat(testFloat);

	ASSERT_EQ((float) 0x3456789A, result);
}

TEST(TestACPPacket2, testDouble){
	uint8 testDouble[] = {0x01, 0x01, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};
	double result = GetDouble(testDouble);

	ASSERT_EQ((float) 0x3456789A, result);
}
