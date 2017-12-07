/*
 * test_GPIOManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: fsw
 */

#include "hal/GPIOManager.h"
#include "test/testmacros.h"
#include <iostream>
#include "test/catch.hpp"

using namespace std;

TEST_CASE("GPIO Output", "[.][hardware][gpio]"){
	cout << "------------------GPIO Output Test---------------------" << endl;
	//Configure the desired GPIOs
	GPIOManager test;
	char bank = 'A';
	int pin = 11;
	GPIOType type = GPIO_INPUT;
	int id = test.attachDevice(bank,pin,type);
	test.initialize();

	GPIOLevel level1 = HIGH;
	test.set(id,level1);
	PROMPT("Setting GPIO at" << bank << pin << " HIGH");

	GPIOLevel level2 = LOW;
	test.set(id,level2);
	PROMPT("Setting GPIO at " << bank << pin << " LOW");
}

TEST_CASE("GPIO Input", "[.][hardware][gpio]"){
	cout << "------------------GPIO Input Test---------------------" << endl;
	GPIOManager test;
	char bank = GPIO_BANK_B;
	int pin = 11;
	GPIOType type = GPIO_OUTPUT;
	int id = test.attachDevice(bank,pin,type);
	test.initialize();

	PROMPT("Ready to begin GPIO Input test...");

	for(int i = 0; i < 10; i++){
		GPIOLevel lev = test.get(id);

		if(lev == HIGH){
			cout << "Read HIGH from " << bank << pin << endl;
			cout.flush();
		}else{
			cout << "Read LOW from " << bank << pin << endl;
			cout.flush():
		}
	}

}
