/*
 * test_SPIManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: fsw
 */

#include "hal/SPIManager.h"
#include <iostream>

using namespace std;


void test_SPITXRXByte(void);

void test_SPIManager(void){
	cout << "Entering Testing for SPIManager" << endl;
	test_SPITXRXByte();
	cout << "Exiting Testing for SPIManager" << endl;

}

//test send byte and receive byte functions
void test_SPITXRXByte(void){

	string bus = "/dev/spidev32765";
	uint8_t mode = 0;
	uint32_t speed = 1000000;
	SPIManager test(bus,mode,speed);

	test.initialize();

	int ss = 1;
	int id = test.attachDevice(ss);

	uint8_t byte = 0x76;
	test.sendbyte(id,byte);

	uint8_t returnByte = test.receivebyte(id);

	if (returnByte == byte){
		cout << "." << endl;
	}else{
		cout << "Failed to receive byte from Subsystem: " << ss << endl;
	}

}

