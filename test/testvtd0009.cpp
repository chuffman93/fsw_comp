/*! \file testvtd0009.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *
 *  \brief Test 9 of the VariableTypeData Class.
 *
 *  This test verifies that the copy constructor and assignment operators
 *  correctly copy both the type and the data.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/CommandMessage.h"
#include "core/ACPPacket.h"

#include "core/VariableTypeData.h"
using namespace std;
using namespace AllStar::Core;

TEST(TestVariableTypeData, CopyAndAssignment) {
	VariableTypeData * vin[VAR_TYPE_ENUM_NUM_TYPES],
			vout[VAR_TYPE_ENUM_NUM_TYPES], *voutcp[VAR_TYPE_ENUM_NUM_TYPES];
	vector<uint8> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(5);

	CommandMessage cmd(1);
	ACPPacket p1(1, 2, 0, 0, &cmd);

	vin[VAR_TYPE_ENUM_INT] = new VariableTypeData((int32) -10);
	vin[VAR_TYPE_ENUM_UNSIGNED_INT] = new VariableTypeData((uint32) 10);
	vin[VAR_TYPE_ENUM_BOOL] = new VariableTypeData(true);
	vin[VAR_TYPE_ENUM_FLOAT] = new VariableTypeData((float) -5.6);
	vin[VAR_TYPE_ENUM_DOUBLE] = new VariableTypeData((double) 100.3);
	vin[VAR_TYPE_ENUM_STRING] = new VariableTypeData("Hello");
	vin[VAR_TYPE_ENUM_ARRAY] = new VariableTypeData(vec);
	vin[VAR_TYPE_ENUM_PACKET] = new VariableTypeData(p1);

	for (size_t t = VAR_TYPE_ENUM_INT; t < VAR_TYPE_ENUM_NUM_TYPES; ++t) {
		voutcp[t] = new VariableTypeData(*vin[t]);

		if (voutcp[t]->GetType() != (vin[t])->GetType()) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
		//cout << "t: " << t << "GetType passed" << endl;

		if (!(*voutcp[t] == *vin[t])) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
		//cout << "t: " << t << "equals operator passed" << endl;

		vout[t] = *vin[t];

		if (vout[t].GetType() != (vin[t])->GetType()) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
		//cout << "t: " << t << "After assignment operator, GetType passed" << endl;

		if (!(vout[t] == *vin[t])) {
			cout << "FAIL AT " << t << endl;
			ASSERT_TRUE(false);
		}
		//cout << "t: " << t << "After assignment operator, GetType passed" << endl;
	}

	if (*((int32 *) (voutcp[VAR_TYPE_ENUM_INT]->GetData()))
			!= *((int32 *) (vin[VAR_TYPE_ENUM_INT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_INT;
		ASSERT_TRUE(false);
	}
	if (*((uint32 *) (voutcp[VAR_TYPE_ENUM_UNSIGNED_INT]->GetData()))
			!= *((uint32 *) (vin[VAR_TYPE_ENUM_UNSIGNED_INT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_UNSIGNED_INT;
		ASSERT_TRUE(false);
	}
	if (*((bool *) (voutcp[VAR_TYPE_ENUM_BOOL]->GetData()))
			!= *((bool *) (vin[VAR_TYPE_ENUM_BOOL]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_BOOL;
		ASSERT_TRUE(false);
	}
	if (*((float *) (voutcp[VAR_TYPE_ENUM_FLOAT]->GetData()))
			!= *((float *) (vin[VAR_TYPE_ENUM_FLOAT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_FLOAT;
		ASSERT_TRUE(false);
	}
	if (*((double *) (voutcp[VAR_TYPE_ENUM_DOUBLE]->GetData()))
			!= *((double *) (vin[VAR_TYPE_ENUM_DOUBLE]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_DOUBLE;
		ASSERT_TRUE(false);
	}
	if (*((string *) (voutcp[VAR_TYPE_ENUM_STRING]->GetData()))
			!= *((string *) (vin[VAR_TYPE_ENUM_STRING]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_STRING;
		ASSERT_TRUE(false);
	}
	if (*((vector<uint8> *) (voutcp[VAR_TYPE_ENUM_ARRAY]->GetData()))
			!= *((vector<uint8> *) (vin[VAR_TYPE_ENUM_ARRAY]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_ARRAY;
		ASSERT_TRUE(false);
	}

	if (*((int32 *) (vout[VAR_TYPE_ENUM_INT].GetData()))
			!= *((int32 *) (vin[VAR_TYPE_ENUM_INT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_INT;
		ASSERT_TRUE(false);
	}
	if (*((uint32 *) (vout[VAR_TYPE_ENUM_UNSIGNED_INT].GetData()))
			!= *((uint32 *) (vin[VAR_TYPE_ENUM_UNSIGNED_INT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_UNSIGNED_INT;
		ASSERT_TRUE(false);
	}
	if (*((bool *) (vout[VAR_TYPE_ENUM_BOOL].GetData()))
			!= *((bool *) (vin[VAR_TYPE_ENUM_BOOL]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_BOOL;
		ASSERT_TRUE(false);
	}
	if (*((float *) (vout[VAR_TYPE_ENUM_FLOAT].GetData()))
			!= *((float *) (vin[VAR_TYPE_ENUM_FLOAT]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_FLOAT;
		ASSERT_TRUE(false);
	}
	if (*((double *) (vout[VAR_TYPE_ENUM_DOUBLE].GetData()))
			!= *((double *) (vin[VAR_TYPE_ENUM_DOUBLE]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_DOUBLE;
		ASSERT_TRUE(false);
	}
	if (*((string *) (vout[VAR_TYPE_ENUM_STRING].GetData()))
			!= *((string *) (vin[VAR_TYPE_ENUM_STRING]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_STRING;
		ASSERT_TRUE(false);
	}
	if (*((vector<uint8> *) (vout[VAR_TYPE_ENUM_ARRAY].GetData()))
			!= *((vector<uint8> *) (vin[VAR_TYPE_ENUM_ARRAY]->GetData()))) {
		cout << "FAIL AT " << VAR_TYPE_ENUM_ARRAY;
		ASSERT_TRUE(false);
	}

	for (size_t t = VAR_TYPE_ENUM_INT; t < VAR_TYPE_ENUM_NUM_TYPES; ++t) {
		delete vin[t];
		delete voutcp[t];
	}
}
