/*
 * test_GPIOManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: fsw
 */

#include "hal/GPIOManager.h"
#include <iostream>

using namespace std;

void test_GPIOInput(void);
void test_GPIOOutput(void);

void test_GPIOManager(void){
	cout << "Entering Testing for GPIOManager" << endl;
	test_GPIOInput();
	test_GPIOOutput();
	cout << "Exiting Testing for GPIOManager" << endl;

}

void test_GPIOInput(void){
	GPIOManager test;

	test.initialize();

	char bank = GPIO_BANK_A;
	int pin = 11;
	GPIOType type = GPIO_INPUT;

	int id = test.attachDevice(bank,pin,type);

	GPIOLevel level1 = HIGH;

	test.set(id,level1);
	GPIOLevel returnLevel1 = test.get(id);

	if (returnLevel1 == level1){
		cout << "." << endl;
	}else{
		cout << "Failed to set bank " << bank << " input to HIGH" << endl;
	}

	GPIOLevel level2 = LOW;

	test.set(id,level2);
	GPIOLevel returnLevel2 = test.get(id);

	if (returnLevel2 == level2){
		cout << "." << endl;
	}else{
		cout << "Failed to set bank " << bank << " input to LOW" << endl;
	}
}


void test_GPIOOutput(void){
	GPIOManager test;

	test.initialize();

	char bank = GPIO_BANK_B;
	int pin = 11;
	GPIOType type = GPIO_OUTPUT;

	int id = test.attachDevice(bank,pin,type);

	GPIOLevel level1 = HIGH;

	test.set(id,level1);
	GPIOLevel returnLevel1 = test.get(id);

	if (returnLevel1 == level1){
		cout << "." << endl;
	}else{
		cout << "Failed to set bank " << bank << " output to HIGH" << endl;
	}

	GPIOLevel level2 = LOW;

	test.set(id,level2);
	GPIOLevel returnLevel2 = test.get(id);

	if (returnLevel1 == level1){
		cout << "." << endl;
	}else{
		cout << "Failed to set bank " << bank << " output to LOW" << endl;
	}
}
