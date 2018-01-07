/*
 * test_ByteStream.cpp
 *
 *  Created on: Jan 7, 2018
 *      Author: alpa3119
 */

#include "test/catch.hpp"
#include "util/ByteStream.h"
#include "test/testmacros.h"

TEST_CASE("BYTESTREAM: TEST SERIALIZE", "[bytestream]"){
	uint8_t data1 = 0x01;
	uint16_t data2 = 0x0102;
	uint32_t data3 = 0x01020304;
	uint64_t data4 = 0x0102030405060708;
	int8_t data5 = 0x01;
	int16_t data6 = 0x0102;
	int32_t data7 = 0x01020304;
	int64_t data8 = 0x0102030405060708;
	float data9 = 2.3879393e-38;
	double data10 = 8.2078803991318400e-304;

	VECTOROFDATA(correct, uint8_t, 0x01, 0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);

	ByteStream bs;
	bs << data1 << data2 << data3 << data4 << data5 << data6 << data7 << data8 << data9 << data10;

	REQUIRE(bs.vec() == correct);
}

TEST_CASE("BYTESTREAM: TEST DESERIALIZE", "[bytestream]"){
	uint8_t data1 = 0x01;
	uint16_t data2 = 0x0102;
	uint32_t data3 = 0x01020304;
	uint64_t data4 = 0x0102030405060708;
	int8_t data5 = 0x01;
	int16_t data6 = 0x0102;
	int32_t data7 = 0x01020304;
	int64_t data8 = 0x0102030405060708;
	float data9 = 2.3879393e-38;
	double data10 = 8.2078803991318400e-304;

	VECTOROFDATA(test, uint8_t, 0x01, 0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);

	ByteStream bs(test);

	uint8_t dat1;
	uint16_t dat2;
	uint32_t dat3;
	uint64_t dat4;
	int8_t dat5;
	int16_t dat6;
	int32_t dat7;
	int64_t dat8;
	float dat9;
	double dat10;

	SECTION("TEST DESERIALIZE WITHOUT SEAK"){

		bs >> dat1 >> dat2 >> dat3 >> dat4 >> dat5 >> dat6 >> dat7 >> dat8 >> dat9 >> dat10;

		REQUIRE(data1 == dat1);
		REQUIRE(data2 == dat2);
		REQUIRE(data3 == dat3);
		REQUIRE(data4 == dat4);
		REQUIRE(data5 == dat5);
		REQUIRE(data6 == dat6);
		REQUIRE(data7 == dat7);
		REQUIRE(data8 == dat8);
		REQUIRE(data9 == dat9);
		REQUIRE(data10 == dat10);

	}
	SECTION("TEST DESERIALIZE WITH SEAK"){
		bs.seek(3) >> dat3 >> dat4 >> dat5 >> dat6 >> dat7 >> dat8 >> dat9 >> dat10;

		REQUIRE(data3 == dat3);
		REQUIRE(data4 == dat4);
		REQUIRE(data5 == dat5);
		REQUIRE(data6 == dat6);
		REQUIRE(data7 == dat7);
		REQUIRE(data8 == dat8);
		REQUIRE(data9 == dat9);
		REQUIRE(data10 == dat10);
	}


}

