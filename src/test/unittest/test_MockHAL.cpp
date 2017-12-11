/*
 * test_MockHAL.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include "test/catch.hpp"
#include <iostream>
#include "test/mockhal/MockSPIManager.h"
#include "test/mockhal/MockGPIOManager.h"
using namespace std;

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
