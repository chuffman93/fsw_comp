/*
 * test_MockHAL.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "test/catch.hpp"
#include "test/testmacros.h"
#include <iostream>
#include <sstream>
#include "test/mockhal/MockSPIManager.h"
#include "test/mockhal/MockGPIOManager.h"
#include "test/mockhal/MockOneWireManager.h"
#include "test/mockhal/MockI2CManager.h"
using namespace std;

TEST_CASE("Test that the MockI2CManager simulates register and raw transactions", "[MockHAL][MockI2CManager]"){
	MockI2CManager i2c;
	int id = i2c.attachDevice(0);
	VECTOROFDATA(testdata, uint8_t, 1,2,3);
	INFO("Setup device");
	i2c.writeReg(id, 10, 100);
	i2c.setRead(id, testdata);
	SECTION("Ensure that read of uninitialized register returns zero"){
		REQUIRE(i2c.readReg(id, 0) == 0);
	}
	SECTION("Ensure that read of initialized register returns value"){
		REQUIRE(i2c.readReg(id, 10) == 100);
	}
	SECTION("Ensure that raw read reads correct data"){
		REQUIRE(i2c.readRaw(id, 3) == testdata);
	}
	SECTION("Ensure that rawwrite sets data correctly"){
		VECTOROFDATA(raw, uint8_t, 0x05);
		i2c.writeRaw(id, raw);
		REQUIRE(i2c.getWritten(id) == raw);
	}
}

TEST_CASE("Test that MockOneWireManager handles data strings", "[MockHAL][MockOneWireManager]"){
	vector<int> devices;
	MockOneWireManager onewire;
	float temp;

	for(int i = 0; i < 16; i++){
		devices.push_back(onewire.attachDevice(""));
	}

	INFO("Check that a blank string is returned if no data is specified");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		CAPTURE(dev);
		REQUIRE(onewire.readFromFile(*dev, "temp") == "");
	}

	INFO("Set each device to have a unique string");
	temp = 15.0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		stringstream ss;
		ss << temp;
		onewire.setData(*dev, ss.str());
		temp += 1;
	}

	INFO("Validate the strings");
	temp = 15.0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		stringstream ss;
		ss << temp;
		REQUIRE(onewire.readFromFile(*dev, "temp") == ss.str());
		temp += 1;
	}
}

TEST_CASE("Test that MockSPIManager properly simulates the SPI Bus", "[MockHAL][MockSPIManager]"){
	vector<int> devices;
	MockSPIManager spi;
	uint8_t byte;
	//Setup the device ids
	for(int i = 0; i < 4; i++){
		devices.push_back(spi.attachDevice(i));
	}

	INFO("Check that receive will return 0s if there is nothing in the queue");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		CAPTURE(dev);
		REQUIRE(spi.receivebyte(*dev) == 0);
	}

	INFO("Test that MockSPIManager can simulate reception of bytes");
	INFO("Insert 10 bytes of unique data for each device");
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		vector<uint8_t> bytes;
		for(int i = 0; i < 10; i++){
			bytes.push_back(byte);
			byte++;
		}
		spi.addBytes(*dev, bytes);
	}
	INFO("Check that all of the bytes are available to be received");
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		for(int i = 0; i < 10; i++){
			CAPTURE(*dev);
			CAPTURE(i);
			REQUIRE(spi.receivebyte(*dev) == byte);
			byte++;
		}
	}

	INFO("Test that MockSPIManager captures sent data");
	INFO("Send 10 bytes of unique data for each device");
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		for(int i = 0; i < 10; i++){
			spi.sendbyte(*dev, byte);
			byte++;
		}
	}

	INFO("Check that all bytes can be read back correctly");
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		CAPTURE(dev);
		vector<uint8_t> bytes = spi.getBytes(*dev);
		REQUIRE(bytes.size() == 10);
		for(int i = 0; i < 10; i++){
			REQUIRE(bytes[i] == byte);
			byte++;
		}
	}

	INFO("Check that receive will return 0s if there is nothing in the queue");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		CAPTURE(dev);
		REQUIRE(spi.receivebyte(*dev) == 0);
	}
}

TEST_CASE("Test that MockInterruptManager simulates interrupts properly", "[MockHAL][MockInterruptManager]"){
	vector<int> devices;
	MockGPIOManager intr;
	for(int i = 0; i < 4; i++){
		devices.push_back(intr.attachDevice('C', 'U', INT_RISING));
	}

	INFO("Check that wait returns false when there are no pending interrupts");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		REQUIRE(intr.wait(*dev, 0) == false);
	}

	INFO("Queue 10 interrupts for each device");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		for(int i = 0; i < 10; i++){
			intr.addpending(*dev);
		}
	}

	INFO("Check that all interrupts get through properly");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		for(int i = 0; i < 10; i++){
			REQUIRE(intr.wait(*dev, 0) == true);
		}
	}

	INFO("Check that wait returns false when there are no pending interrupts");
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		REQUIRE(intr.wait(*dev, 0) == false);
	}

}
