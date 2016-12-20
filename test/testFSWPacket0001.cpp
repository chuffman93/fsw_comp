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

/*void createMessageBuffer(uint8 * buffer, int size){
	for(int i = 0; i < 10; i++){
		buffer[i] = 0x00;
	}
	buffer[10] = 0b11000011; // status
	buffer[11] = EPS_HS_SUCCESS; // sample opcode
	buffer[12] = 0x00; // length
	buffer[13] = size-16; // length

	for(int i = 14; i < size - 2; i++){
		buffer[i] = 0x06;
	}
	buffer[16] = 0x0C; // CRC
	buffer[17] = 0x0C; // CRC
}

TEST(TestACPPacket, testPacket){
	uint8 * testBuf;
	testBuf = (uint8 *) malloc(18);

	createMessageBuffer(testBuf, 18);

	ACPPacket * testPacket = new ACPPacket(testBuf, 18);

	ASSERT_EQ(testPacket->GetMessageBufPtr()[0], 0x06);
	ASSERT_EQ(testPacket->GetMessageBufPtr()[1], 0x06);
	ASSERT_EQ(testPacket->GetMessageLength(), 2);
	ASSERT_EQ(testPacket->GetFlattenSize(), 18);

	free(testBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}

TEST(TestACPPacket, testNullMessage){
	uint8 * testBuf;
	testBuf = (uint8 *) malloc(16);

	createMessageBuffer(testBuf, 16);

	ACPPacket * testPacket = new ACPPacket(testBuf, 16);

	ASSERT_EQ(testPacket->GetMessageLength(), 0);
	ASSERT_EQ(testPacket->GetFlattenSize(), 16);

	free(testBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}

TEST(TestACPPacket, testFlatten){
	uint8 * testBuf;
	uint8 * flattenBuf;
	size_t size = 18;
	testBuf = (uint8 *) malloc(size);
	flattenBuf = (uint8 *) malloc(size);

	createMessageBuffer(testBuf, size);

	ACPPacket * testPacket = new ACPPacket(testBuf, size);

	size_t result = testPacket->Flatten(flattenBuf, testPacket->GetFlattenSize());
	EXPECT_EQ(size, testPacket->GetFlattenSize());
	EXPECT_EQ(size, result);


	bool equal = true;
	for(size_t i = 0; i < size-2; i++){ //ignore CRC for now
		equal &= (flattenBuf[i] == testBuf[i]);
	}

	ASSERT_TRUE(equal);

	free(testBuf);
	free(flattenBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}

TEST(TestACPPacket, testBadLength){
	uint8 * testBuf;
	size_t size = 18;
	testBuf = (uint8 *) malloc(size);

	createMessageBuffer(testBuf, size);
	testBuf[13] = 0x06; //incorrect length of message

	ACPPacket * testPacket = new ACPPacket(testBuf, size);

	ASSERT_TRUE(testPacket->GetMessageBufPtr()==NULL);
	ASSERT_EQ(testPacket->GetDestination(), LOCATION_ID_INVALID);
	ASSERT_EQ(testPacket->GetSource(), LOCATION_ID_INVALID);

	free(testBuf);
	delete testPacket;
	ASSERT_TRUE(true);
}

TEST(TestACPPacket, testPacketLog){
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	uint8 * testBuf;
	testBuf = (uint8 *) malloc(18);

	createMessageBuffer(testBuf, 18);

	ACPPacket * testPacket = new ACPPacket(testBuf, 18);

	bool success = fileHandler->Log(SUBSYSTEM_EPS, testPacket);
	ASSERT_TRUE(success);

	free(testBuf);
	delete testPacket;
}

TEST(TestACPPacket, testPacketProcess){
	uint8 * testBuf;
	testBuf = (uint8 *) malloc(18);

	createMessageBuffer(testBuf, 18);

	ACPPacket * testPacket = new ACPPacket(testBuf, 18);

	AllStar::Servers::PacketProcess(SERVER_LOCATION_EPS, testPacket);

	free(testBuf);
	delete testPacket;
}*/
