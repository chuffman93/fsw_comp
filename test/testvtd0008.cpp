/*! \file testvtd0008.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 8 of the VariableTypeData Class.
 *
 *  This test verifies that Flatten() and the buffer constructor work
 *  correctly.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/VariableTypeData.h"
#include "core/ACPPacket.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

#define BUFFER_SIZE	100
#define TEST_STRING "Testing!"

TEST(TestVariableTypeData, BufferConstructorAndFlatten) {
	uint8 * buffer = new uint8[BUFFER_SIZE];
	size_t expectedSizes[] = { 0, 1 + sizeof(int32), 1 + sizeof(uint32), 1
			+ sizeof(bool), 1 + sizeof(float), 1 + sizeof(double), 1
			+ sizeof(uint32) + sizeof(TEST_STRING) - 1, 0, 0 };
	size_t flattenSize;
	VariableTypeData * vin[VAR_TYPE_ENUM_NUM_TYPES],
			*vout[VAR_TYPE_ENUM_NUM_TYPES];
	vector<uint8> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(5);
	expectedSizes[VAR_TYPE_ENUM_ARRAY] = 1 + vec.size() + sizeof(uint32);

	CommandMessage cmd(1);
	ACPPacket p1(1, 2, 0, 0, &cmd);
	expectedSizes[VAR_TYPE_ENUM_PACKET] = 1 + p1.GetFlattenSize();

	vin[VAR_TYPE_ENUM_INT] = new VariableTypeData((int32) -10);
	vin[VAR_TYPE_ENUM_UNSIGNED_INT] = new VariableTypeData((uint32) 10);
	vin[VAR_TYPE_ENUM_BOOL] = new VariableTypeData(true);
	vin[VAR_TYPE_ENUM_FLOAT] = new VariableTypeData((float) -5.6);
	vin[VAR_TYPE_ENUM_DOUBLE] = new VariableTypeData((double) 100.3);
	vin[VAR_TYPE_ENUM_STRING] = new VariableTypeData(TEST_STRING);
	vin[VAR_TYPE_ENUM_ARRAY] = new VariableTypeData(vec);
	vin[VAR_TYPE_ENUM_PACKET] = new VariableTypeData(p1);

	for (size_t t = VAR_TYPE_ENUM_INT; t < VAR_TYPE_ENUM_NUM_TYPES; ++t) {
		flattenSize = vin[t]->Flatten(NULL, vin[t]->GetFlattenSize());
		if (flattenSize != 0) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
		// 				cout << "t: " << t
		// 					 << " GetFlattenSize( ): " << vin[t]->GetFlattenSize( )
		// 					 << " Expected: " << expectedSizes[t] << endl;
		flattenSize = vin[t]->Flatten(buffer, vin[t]->GetFlattenSize() - 1);

		if (flattenSize != 0) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}

		flattenSize = vin[t]->Flatten(buffer, BUFFER_SIZE);

//		cout << "buffer: " << int(buffer[0]) << " " << int(buffer[1]) << " "
//			 << int(buffer[2]) << " " << int(buffer[3]) << " " << int(buffer[4]) << endl;

		if (vin[t]->GetFlattenSize() != expectedSizes[t]) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}

		// 				cout << "t: " << t << " Flatten Size: " << flattenSize
		// 					 << " Expected: " << vin[t]->GetFlattenSize( ) << endl;
		if (flattenSize != vin[t]->GetFlattenSize()) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}

		// Check that if the buffer is not sized right, then the constructor
		// sets the type to void.
		vout[t] = new VariableTypeData(buffer, vin[t]->GetFlattenSize() - 1);
		//				cout << "Type: " << vout[t]->GetType( ) << endl;
		if (vout[t]->GetType() != VAR_TYPE_ENUM_VOID) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
		delete vout[t];

		vout[t] = new VariableTypeData(buffer, vin[t]->GetFlattenSize());
		if (vout[t]->GetType() != vin[t]->GetType()) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
	}

	if (*((int32 *) (vout[VAR_TYPE_ENUM_INT]->GetData()))
			!= *((int32 *) (vin[VAR_TYPE_ENUM_INT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_INT;
		ASSERT_TRUE(false);
	}
	if (*((uint32 *) (vout[VAR_TYPE_ENUM_UNSIGNED_INT]->GetData()))
			!= *((uint32 *) (vin[VAR_TYPE_ENUM_UNSIGNED_INT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_UNSIGNED_INT;
		ASSERT_TRUE(false);
	}
	if (*((bool *) (vout[VAR_TYPE_ENUM_BOOL]->GetData()))
			!= *((bool *) (vin[VAR_TYPE_ENUM_BOOL]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_BOOL;
		ASSERT_TRUE(false);
	}
	if (*((float *) (vout[VAR_TYPE_ENUM_FLOAT]->GetData()))
			!= *((float *) (vin[VAR_TYPE_ENUM_FLOAT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_FLOAT;
		ASSERT_TRUE(false);
	}
	if (*((double *) (vout[VAR_TYPE_ENUM_DOUBLE]->GetData()))
			!= *((double *) (vin[VAR_TYPE_ENUM_DOUBLE]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_DOUBLE;
		ASSERT_TRUE(false);
	}
	if (*((string *) (vout[VAR_TYPE_ENUM_STRING]->GetData()))
			!= *((string *) (vin[VAR_TYPE_ENUM_STRING]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_STRING;
		ASSERT_TRUE(false);
	}
	if (*((vector<uint8> *) (vout[VAR_TYPE_ENUM_ARRAY]->GetData()))
			!= *((vector<uint8> *) (vin[VAR_TYPE_ENUM_ARRAY]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_ARRAY;
		ASSERT_TRUE(false);
	}

	cout << __LINE__ << endl;

	if (!(*((ACPPacket *) (vout[VAR_TYPE_ENUM_PACKET]->GetData()))
			== *((ACPPacket *) (vin[VAR_TYPE_ENUM_PACKET]->GetData())))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_PACKET;
		ASSERT_TRUE(false);
	}

	delete buffer;
	for (size_t t = VAR_TYPE_ENUM_INT; t < VAR_TYPE_ENUM_NUM_TYPES; ++t) {
		delete vin[t];
		delete vout[t];
	}
}
