/*
 * test_SPIManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: fsw
 */

#include "../include/hal/SPIManager.h"
#include <iostream>

using namespace std;

void test_SPIManager(void){
	cout << "Testing SPIManager" << endl;

}


//test constructor
void test_SPIConstructor(void){
	string base = 'BUS';
	uint8_t mode = 0;
	uint32_t speed = 115200;
	SPIManager test(base, mode, speed);
	string b = test.getBase();
	uint8_t m = test.getMode();
	uint32_t s = test.getSpeed();
	if (b == base) {
		cout << "." << endl;
	}else{
		cout << "Failed to construct SPIManager busbase" << endl;
	}
	if (m == mode) {
		cout << "." << endl;
	}else{
		cout << "Failed to construct SPIManager mode" << endl;
	}
	if (s == speed) {
		cout << "." << endl;
	}else{
		cout << "Failed to construct SPIManager speed" << endl;
	}
}

//test initialization function
void test_SPIInit(void){

}

//test attach device function
void test_SPIAttachDevice(void){

}

//test send byte function
void test_SPISendByte(void){

}


//test receive byte function
void test_SPIReceiveByte(void)
{

}
