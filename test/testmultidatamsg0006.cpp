/*! \file testmultdatmsg0006.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 6 of the MultiDataMessage Class.
 *
 *      This test verifies that the SetParameters method from
 *      a buffer of the MultiDataClass works properly
 *      for both Config and Command messages.
 *
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/CommandMessage.h"
#include "core/ConfigMessage.h"
#include "core/ErrorMessage.h"
#include "core/DataMessage.h"

using namespace std;
using namespace AllStar::Core;
using namespace rel_ops;

#define TEST_OPCODE1 ((MessageCodeType)37)
#define TEST_VALUE1 ((double)11.1)
#define TEST_VALUE2 "Goodbye"
#define TEST_VALUE3 false
#define TEST_VALUE4 ((uint32)7654321)

#define BUFFER_SIZE 100

TEST(TestMultiDataMessage, SetParameters) {
	uint8 * buffer = new uint8[BUFFER_SIZE];

	// Messages that will be used to see if data was set correctly
	CommandMessage cm1(TEST_OPCODE1);
	ConfigMessage cf1(TEST_OPCODE1);
	ErrorMessage e1(TEST_OPCODE1);
	DataMessage d1(TEST_OPCODE1);

	// Data that will be used to set
	VariableTypeData v1(TEST_VALUE1);
	VariableTypeData v2(TEST_VALUE2);

	// Data list that will be used to set
	list<VariableTypeData *> l1;

	// Add data to data list
	l1.push_back(&v1);
	l1.push_back(&v2);

	// START CREATING BUFFER

	// Get the size of the numParameters byte, and the two VariableTypeDatas
	size_t size = sizeof(uint8) + v1.GetFlattenSize() + v2.GetFlattenSize();

	// Set length of the message equal to current size
	uint16 length = size;

	// Update size
	size += sizeof(uint16);

	// Put length into the buffer
	*buffer++ = (length >> 8) & 0xFF;
	*buffer++ = length & 0xFF;

	// Put the number of parameters into the buffer
	*buffer++ = ((uint8) 2);

	// Put the first data into the buffer
	v1.Flatten(buffer, size);

	// Update the buffer
	buffer += v1.GetFlattenSize();

	// Put the second data into the buffer
	v2.Flatten(buffer, size);

	// Return the buffer pointer to the beginning
	buffer -= v1.GetFlattenSize();
	buffer -= sizeof(uint16);
	buffer--;

	// BUFFER CREATED

	// Set the parameters from the buffer
	cm1.SetParameters(buffer, size);
	cf1.SetParameters(buffer, size);
	e1.SetParameters(buffer, size);
	d1.SetParameters(buffer, size);

	// Create a new message with the correct message
	CommandMessage cm2(TEST_OPCODE1, l1);
	ConfigMessage cf2(TEST_OPCODE1, l1);
	ErrorMessage e2(TEST_OPCODE1, l1);
	DataMessage d2(TEST_OPCODE1, l1);

	// Check that the parameters have been set correctly
	if (!(cm1.ParametersEqual(cm2)) || !(cf1.ParametersEqual(cf2))
			|| !(e1.ParametersEqual(e2)) || !(d1.ParametersEqual(d1))) {
		cout
				<< "MultiDataMessage failed to set parameters from the buffer correctly.";

		ASSERT_TRUE(false);
	}

	delete buffer;
}
