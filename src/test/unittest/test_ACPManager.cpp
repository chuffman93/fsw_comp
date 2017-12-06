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

}

void test_PacketTransaction(){
	cout << "PacketTransaction ";

}

void test_ACPManager(){
	cout << "ACPManager " << endl;
}

