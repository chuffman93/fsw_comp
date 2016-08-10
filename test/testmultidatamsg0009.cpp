/*! \file testmultdatmsg0009.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 9 of the MultiDataMessage Class.
 *
 *      This test verifies that the Flatten Method of the
 *      MultiDataMessage class works correctly for a ConfigMessage
 *      without list parameter.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ConfigMessage.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"
#include "core/Message.h"

using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define TEST_OPCODE1 ((MessageCodeType)76)
#define TEST_VALUE1 ((double)27.2)
#define TEST_VALUE2 "Hello"

#define BUFFER_SIZE 100

TEST(TestMultiDataMessage, Flatten) {
	uint8 * buffer = new uint8[BUFFER_SIZE];

	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	list<VariableTypeData *> l1;

	l1.push_back(&v1);
	l1.push_back(&v2);

	// Message to be flattened
	ConfigMessage cfin(TEST_OPCODE1, l1);
	CommandMessage cmin(TEST_OPCODE1, l1);
	DataMessage din(TEST_OPCODE1, l1);
	ErrorMessage ein(TEST_OPCODE1, l1);

	// Try to flatten with a null buffer
	size_t cfNumCopied = cfin.Flatten(NULL, cfin.GetFlattenSize());
	size_t cmNumCopied = cmin.Flatten(NULL, cmin.GetFlattenSize());
	size_t dNumCopied = din.Flatten(NULL, din.GetFlattenSize());
	size_t eNumCopied = ein.Flatten(NULL, ein.GetFlattenSize());

	// Make sure that a null buffer returns 0
	if (cfNumCopied != 0 || cmNumCopied != 0 || dNumCopied != 0
			|| eNumCopied != 0) {
		cout << "Flatten method did return 0 when the buffer "
				"parameter was null.";

		ASSERT_TRUE(false);
	}

	// Try to flatten with insufficiently large size
	cfNumCopied = cfin.Flatten(buffer, cfin.GetFlattenSize() - 1);
	cmNumCopied = cmin.Flatten(buffer, cmin.GetFlattenSize() - 1);
	dNumCopied = din.Flatten(buffer, din.GetFlattenSize() - 1);
	eNumCopied = ein.Flatten(buffer, ein.GetFlattenSize() - 1);

	// Make sure that small size returns 0
	if (cfNumCopied != 0 || cmNumCopied != 0 || dNumCopied != 0
			|| eNumCopied != 0) {
		cout << "Flatten method did return 0 when the size was "
				"less than GetFlattenSize().";

		ASSERT_TRUE(false);
	}

	uint8 * buffer1 = new uint8[BUFFER_SIZE];
	uint8 * buffer2 = new uint8[BUFFER_SIZE];
	uint8 * buffer3 = new uint8[BUFFER_SIZE];
	uint8 * buffer4 = new uint8[BUFFER_SIZE];

	// Flatten the message
	cfNumCopied = cfin.Flatten(buffer1, cfin.GetFlattenSize());
	cmNumCopied = cmin.Flatten(buffer2, cmin.GetFlattenSize());
	dNumCopied = din.Flatten(buffer3, din.GetFlattenSize());
	eNumCopied = ein.Flatten(buffer4, ein.GetFlattenSize());

	// Make sure flatten returns the flatten size
	if (cfNumCopied != cfin.GetFlattenSize()
			|| cmNumCopied != cmin.GetFlattenSize()
			|| dNumCopied != din.GetFlattenSize()
			|| eNumCopied != ein.GetFlattenSize()) {
		cout << "Flatten method did return GetFlattenSize().";

		ASSERT_TRUE(false);
	}

	// Create a message from the flattened buffer
	Message * m1out = Message::CreateMessage(buffer1, cfin.GetFlattenSize());
	Message * m2out = Message::CreateMessage(buffer2, cmin.GetFlattenSize());
	Message * m3out = Message::CreateMessage(buffer3, din.GetFlattenSize());
	Message * m4out = Message::CreateMessage(buffer4, ein.GetFlattenSize());

	// Make sure the created message has the right type
	if (m1out->GetType() != MESSAGE_TYPE_CONFIG
			|| m2out->GetType() != MESSAGE_TYPE_COMMAND
			|| m3out->GetType() != MESSAGE_TYPE_DATA
			|| m4out->GetType() != MESSAGE_TYPE_ERROR) {
		cout << "GetType() did not return correct message type.";

		ASSERT_TRUE(false);
	}

	// Attempt to cast the message into a specific types of message
	ConfigMessage * cfout = dynamic_cast<ConfigMessage *>(m1out);
	CommandMessage * cmout = dynamic_cast<CommandMessage *>(m2out);
	DataMessage * dout = dynamic_cast<DataMessage *>(m3out);
	ErrorMessage * eout = dynamic_cast<ErrorMessage *>(m4out);

	// Check if unsuccessful
	if (cfout == NULL || cmout == NULL || dout == NULL || eout == NULL) {
		cout << "Cast to correct message type was unsuccessful.";

		ASSERT_TRUE(false);
	}

	// Check that flattened and unflattened message is equal to original
	if (!(cfin == cfout) || !(cmin == cmout) || !(din == dout)
			|| !(ein == eout)) {
		cout << "Flatten did not correctly preserve message.";

		ASSERT_TRUE(false);
	}

	delete buffer;
	delete buffer1;
	delete buffer2;
	delete buffer3;
	delete buffer4;
	delete m1out;
	delete m2out;
	delete m3out;
	delete m4out;
}
