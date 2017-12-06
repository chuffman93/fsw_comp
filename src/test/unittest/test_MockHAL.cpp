/*
 * test_MockHAL.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */
#include <iostream>
#include "test/mockhal/MockSPIManager.h"
using namespace std;

//! Creates several test devices, and ensures that the correct number of bytes gets returned from each
void test_MockSPI(){
	vector<int> devices;
	MockSPIManager spi;
	cout << "MockSPI ";
	for(int i = 0; i < 4; i++){
		devices.push_back(spi.attachDevice(i));
	}

	//Check that receive will return 0s if there is nothing in the queue
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		if(spi.receivebyte(*dev) != 0){
			cout << "Failed to return 0 when empty for device" << *dev << endl;
			return;
		}
		cout << ".";
	}

	//Insert 10 bytes of unique data for each device
	uint8_t byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		vector<uint8_t> bytes;
		for(int i = 0; i < 10; i++){
			bytes.push_back(byte);
			byte++;
		}
		spi.addBytes(*dev, bytes);
	}

	//Check that all of the bytes are available to be received
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		for(int i = 0; i < 10; i++){
			if(spi.receivebyte(*dev) != byte){
				cout << "Failed to get the right bytes from device" << *dev << endl;
				return;
			}
			cout << ".";
			byte++;
		}
	}

	//Send 10 bytes of unique data for each device
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		for(int i = 0; i < 10; i++){
			spi.sendbyte(*dev, byte);
			byte++;
		}
	}

	//Check all of the bytes were sent correctly
	byte = 0;
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		vector<uint8_t> bytes = spi.getBytes(*dev);
		if(bytes.size() != 10){
			cout << "Failed to get the right number of bytes from device " << *dev << endl;
		}
		for(int i = 0; i < 10; i++){
			if(bytes[i] != byte){
				cout << "Failed to get correct byte of " << byte << "From device " << *dev << endl;
				return;
			}
			cout << ".";
			byte++;
		}
	}

	//Check that receive will return 0s if there is nothing in the queue
	for(vector<int>::iterator dev = devices.begin(); dev < devices.end(); dev++){
		if(spi.receivebyte(*dev) != 0){
			cout << "Failed to return 0 when empty for device" << *dev << endl;
			return;
		}
		cout << ".";
	}
	cout << endl;
}


void test_MockHAL(){
	cout << "MockHAL " << endl;
	test_MockSPI();
}
