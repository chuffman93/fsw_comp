#include "gtest/gtest.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "core/StdTypes.h"
#include "core/FSWPacket.h"

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;

void createMessageBuffer(uint8 * buffer, int size){
	for(int i = 0; i < 10; i++){
		buffer[i] = 0x00;
	}
	buffer[10] = 0b11000011; // status
	buffer[11] = 0x03; // opcode
	buffer[12] = 0x00; // length
	buffer[13] = size-16; // length

	for(int i = 14; i < size - 2; i++){
		buffer[i] = 0x06;
	}
	buffer[16] = 0x0C; // CRC
	buffer[17] = 0x0C; // CRC
}


TEST(TestFSWPacket, testBuffer){
	uint8 * testBuf;
	testBuf = (uint8 *) malloc(18);

	createMessageBuffer(testBuf, 18);

	FSWPacket * testPacket = new FSWPacket(testBuf, 18);

	ASSERT_EQ(testPacket->GetMessageBufPtr()[0], 0x06);
	ASSERT_EQ(testPacket->GetMessageBufPtr()[1], 0x06);
	ASSERT_EQ(testPacket->GetMessageLength(), 2);
	ASSERT_EQ(testPacket->GetFlattenSize(), 18);

	printf("Element 0: %u\n", testPacket->GetMessageBufPtr()[0]);
	printf("Element 1: %u\n", testPacket->GetMessageBufPtr()[1]);

	free(testBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}

TEST(TestFSWPacket, testNullMessage){
	uint8 * testBuf;
	testBuf = (uint8 *) malloc(16);

	createMessageBuffer(testBuf, 16);

	FSWPacket * testPacket = new FSWPacket(testBuf, 16);

	ASSERT_EQ(testPacket->GetMessageLength(), 0);
	ASSERT_EQ(testPacket->GetFlattenSize(), 16);

	free(testBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}

TEST(TestFSWPacket, testFlatten){
	uint8 * testBuf;
	uint8 * flattenBuf;
	size_t size = 18;
	testBuf = (uint8 *) malloc(size);
	flattenBuf = (uint8 *) malloc(size);

	createMessageBuffer(testBuf, size);

	FSWPacket * testPacket = new FSWPacket(testBuf, size);

	testPacket->Flatten(flattenBuf, testPacket->GetFlattenSize());
	EXPECT_EQ(size, testPacket->GetFlattenSize());


	bool equal = true;
	for(size_t i = 0; i < size; i++){
		equal &= (flattenBuf[i] == testBuf[i]);
		printf("At index %u\n", i);
		printf("Flatten: %d\n", flattenBuf[i]);
		printf("Test:    %d\n", testBuf[i]);
	}

	ASSERT_TRUE(equal);

	free(testBuf);
	free(flattenBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}
