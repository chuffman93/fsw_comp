/*! \file testretmsg0012.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 12 of the ReturnMessage Class.
 *
 *	This test verifies that the Flatten method of
 *	Return Message works correctly for a Return Message
 *	with filled pointer.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ReturnMessage.h"
#include "core/DataMessage.h"
#include "core/ErrorMessage.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

#define TEST_OPCODE1 23

#define TEST_VALUE1 "neato"
#define TEST_VALUE2 true
#define TEST_VALUE3 ((double)12.1)
#define TEST_VALUE4 ((uint32)1234321)

#define TEST_CONFIG1 1
#define TEST_CONFIG2 3
#define TEST_CONFIG3 9
#define TEST_CONFIG4 5

#define BUFFER_SIZE 100

TEST(TestReturnMessage, FlattenWithFilledPointer) {
	uint8 * buffer1 = new uint8[BUFFER_SIZE];
	uint8 * buffer2 = new uint8[BUFFER_SIZE];
	uint8 * buffer3 = new uint8[BUFFER_SIZE];
	uint8 * buffer4 = new uint8[BUFFER_SIZE];

	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);
	VariableTypeData v3(TEST_VALUE3);
	VariableTypeData v4(TEST_VALUE4);

	std::list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);
	l1.push_back(&v3);
	l1.push_back(&v4);

	DataMessage d1(TEST_OPCODE1, l1);
	ErrorMessage e1(TEST_OPCODE1, l1);
	ConfigMessage cf1(TEST_OPCODE1, l1);
	CommandMessage cm1(TEST_OPCODE1, l1);

	ReturnMessage rd1in(&d1, true);
	ReturnMessage re1in(&e1, false);
	ReturnMessage rcf1in(&cf1, true);
	ReturnMessage rcm1in(&cm1, false);

	// Try to flatten with a null buffer
	size_t numCopiedD = rd1in.Flatten(NULL, rd1in.GetFlattenSize());
	size_t numCopiedE = re1in.Flatten(NULL, re1in.GetFlattenSize());
	size_t numCopiedCF = rcf1in.Flatten(NULL, rcf1in.GetFlattenSize());
	size_t numCopiedCM = rcm1in.Flatten(NULL, rcm1in.GetFlattenSize());

	// Make sure that a null buffer returns 0
	if (numCopiedD != 0 || numCopiedE != 0 || numCopiedCF != 0
			|| numCopiedCM != 0) {
		cout << "Flatten method did return 0 when the buffer "
				"parameter was null.";

		ASSERT_TRUE(false);
	}

	// Try to flatten with insufficiently large size
	numCopiedD = rd1in.Flatten(buffer1, rd1in.GetFlattenSize() - 1);
	numCopiedE = re1in.Flatten(buffer2, re1in.GetFlattenSize() - 1);
	numCopiedCF = rcf1in.Flatten(buffer3, rcf1in.GetFlattenSize() - 1);
	numCopiedCM = rcm1in.Flatten(buffer4, rcm1in.GetFlattenSize() - 1);

	// Make sure that small size returns 0
	if (numCopiedD != 0 || numCopiedE != 0 || numCopiedCF != 0
			|| numCopiedCM != 0) {
		cout << "Flatten method did return 0 when the size was "
				"less than GetFlattenSize().";

		ASSERT_TRUE(false);
	}

	// Flatten the message
	numCopiedD = rd1in.Flatten(buffer1, rd1in.GetFlattenSize());
	numCopiedE = re1in.Flatten(buffer2, re1in.GetFlattenSize());
	numCopiedCF = rcf1in.Flatten(buffer3, rcf1in.GetFlattenSize());
	numCopiedCM = rcm1in.Flatten(buffer4, rcm1in.GetFlattenSize());

	// Make sure flatten returns the flatten size
	if (numCopiedD != rd1in.GetFlattenSize()
			|| numCopiedE != re1in.GetFlattenSize()
			|| numCopiedCF != rcf1in.GetFlattenSize()
			|| numCopiedCM != rcm1in.GetFlattenSize()) {
		cout << "Flatten method did return GetFlattenSize().";

		ASSERT_TRUE(false);
	}

	// Create a message from the flattened buffer
	Message * m1out = Message::CreateMessage(buffer1, rd1in.GetFlattenSize());
	Message * m2out = Message::CreateMessage(buffer2, re1in.GetFlattenSize());
	Message * m3out = Message::CreateMessage(buffer3, rcf1in.GetFlattenSize());
	Message * m4out = Message::CreateMessage(buffer4, rcm1in.GetFlattenSize());

	// Make sure the created message has the right type
	if (m1out->GetType() != MESSAGE_TYPE_DATA
			|| m2out->GetType() != MESSAGE_TYPE_ERROR
			|| m3out->GetType() != MESSAGE_TYPE_CONFIG
			|| m4out->GetType() != MESSAGE_TYPE_COMMAND) {
		cout << "GetType() did not return correct message type.";

		ASSERT_TRUE(false);
	}

	// Attempt to cast the message into an error message
	ReturnMessage * r1out = dynamic_cast<ReturnMessage *>(m1out);
	ReturnMessage * r2out = dynamic_cast<ReturnMessage *>(m2out);
	ReturnMessage * r3out = dynamic_cast<ReturnMessage *>(m3out);
	ReturnMessage * r4out = dynamic_cast<ReturnMessage *>(m4out);

	// Check if unsuccessful
	if (r1out == NULL || r2out == NULL || r3out == NULL || r4out == NULL) {
		cout << "Cast to correct message type was unsuccessful.";

		ASSERT_TRUE(false);
	}

	// Check that flattened and unflattened message is equal to original
	if (!(rd1in == r1out) || !(re1in == r2out) || !(rcf1in == r3out)
			|| !(rcm1in == r4out)) {
		cout << "Flatten did not correctly preserve message.";

		ASSERT_TRUE(false);
	}

	delete buffer1;
	delete buffer2;
	delete buffer3;
	delete buffer4;
	delete m1out;
	delete m2out;
	delete m3out;
	delete m4out;
}
