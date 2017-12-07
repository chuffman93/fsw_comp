/*
 * test_ACPManager.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include <iostream>
#include "test/catch.hpp"
using namespace std;
#include "test/testmacros.h"
#include "util/ACPPacket.h"
#include "hal/ACPManager.h"
#include "test/mockhal/MockSPIManager.h"
#include "test/mockhal/MockInterruptManager.h"

TEST_CASE("Test that ACPPacket manages packets correctly", "[core][ACPPacket]"){
	subsystem_sync_t sync = EPS;
	uint16_t id = 0;
	uint8_t  opcode = 10;
	VECTOROFDATA(message, uint8_t, 1,2,3)
	ACPPacket p(sync, opcode, message);

	SECTION("Check that packets are packed correctly"){
		VECTOROFDATA(correct_bytes, uint8_t, 0xA4, 0x00, 0x00, 0x0A, 0x00, 0x03, 0x01, 0x02, 0x03, 0xE0, 0x3C)
		REQUIRE(p.pack() == correct_bytes);
	}
	SECTION("Check that packet is validated correctly"){
		p.crc = 0xE03C;
		REQUIRE(p.validate() == true);
	}
	SECTION("Check that invalid crc is rejected"){
		p.crc = 0x1010;
		REQUIRE(p.validate() == false);
	}
}

TEST_CASE("Test that the ACPManager performs correctly", "[hal][ACPManager]"){
	int spiid, intrid, acpid;
	MockSPIManager spi;
	MockInterruptManager intr;
	ACPManager acp(spi, intr);
	VECTOROFDATA(message, uint8_t, 1,2,3)
	ACPPacket test(EPS, 10, message);
	VECTOROFDATA(correct_bytes, uint8_t, 0xA4, 0x00, 0x00, 0x0A, 0x00, 0x03, 0x01, 0x02, 0x03, 0xE0, 0x3C)

	INFO("Register the Devices");
	spiid = spi.attachDevice(0);
	intrid = intr.attachDevice('C', 'U', INT_RISING);
	acpid = acp.attachDevice(spiid, intrid);

	SECTION("Normal packet send and receive"){
		spi.addBytes(spiid, correct_bytes);
		for(size_t i = 0; i < 2*correct_bytes.size() - 1; i++) intr.addpending(intrid);

		ACPPacket ret;
		REQUIRE(acp.transaction(acpid, test, ret) == true);
		REQUIRE(spi.getBytes(spiid) == correct_bytes);
		REQUIRE(ret.sync == 0xA4);
		REQUIRE(ret.id == 0);
		REQUIRE(ret.message == message);
		REQUIRE(ret.opcode == 10);
		REQUIRE(ret.crc == 0xE03C);
	}

	SECTION("Timeout in transmit"){
		spi.addBytes(spiid, correct_bytes);
		//Load up one too few interrupts
		for(size_t i = 0; i < correct_bytes.size() - 1; i++) intr.addpending(intrid);
		ACPPacket ret;
		REQUIRE(acp.transaction(acpid, test, ret) == false);
	}

	SECTION("Timeout in receive"){
		spi.addBytes(spiid, correct_bytes);
		//Load up one too few interrupts
		for(size_t i = 0; i < 2*correct_bytes.size() - 2; i++) intr.addpending(intrid);
		ACPPacket ret;
		REQUIRE(acp.transaction(acpid, test, ret) == false);
	}

	SECTION("Rejecting bad data"){
		//Corrupt the CRC
		correct_bytes.back() = 0;
		spi.addBytes(spiid, correct_bytes);
		for(size_t i = 0; i < 2*correct_bytes.size(); i++) intr.addpending(intrid);
		ACPPacket ret;
		REQUIRE(acp.transaction(acpid, test, ret) == false);
	}

}

