/*
 * test_GPIOManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: fsw
 */

#include "hal/GPIOManager.h"
#include "test/testmacros.h"
#include <iostream>
#include <unistd.h>
#include "test/catch.hpp"

using namespace std;

TEST_CASE("GPIO Output", "[.][hardware][gpio]"){
	cout << "------------------GPIO Output Test---------------------" << endl;
	//Configure the desired GPIOs
	GPIOManager test("/sys/class/gpio/");
	char bank = 'B';
	int pin = 25;
	GPIODirection direction = GPIO_OUTPUT;
	int id = test.attachDevice(bank, pin, GPIO_OUTPUT);
	test.initialize();

	GPIOLevel level1 = HIGH;
	test.set(id,level1);
	PROMPT("Setting GPIO at " << bank << pin << " HIGH");

	GPIOLevel level2 = LOW;
	test.set(id,level2);
	PROMPT("Setting GPIO at " << bank << pin << " LOW");
}

TEST_CASE("GPIO Input", "[.][hardware][gpio]"){
	cout << "------------------GPIO Input Test---------------------" << endl;
	GPIOManager test("/sys/class/gpio/");
	char bank = 'E';
	int pin = 6;
	GPIODirection direction = GPIO_INPUT;
	int id = test.attachDevice(bank,pin,direction);
	test.initialize();

	PROMPT("Ready to begin GPIO Input test...");

	for(int i = 0; i < 10; i++){
		GPIOLevel lev = test.get(id);

		if(lev == HIGH){
			cout << "Read HIGH from " << bank << pin << endl;
		}else{
			cout << "Read LOW from " << bank << pin << endl;
		}
		usleep(1*1000*1000);
	}

}

TEST_CASE("GPIO Interrupt", "[.][hardware][interrupt]"){
	cout << "------------------GPIO Interrupt Test---------------------" << endl;
	GPIOManager test("/sys/class/gpio/");
	char bank = 'E';
	int pin = 6;
	int id = test.attachDevice(bank,pin,INT_FALLING);
	test.initialize();

	PROMPT("Ready to begin Interrupt test on " << bank << pin);
	for(int i = 0; i < 10; i++){
		if(test.wait(id, 1000)){
			cout << "Received interrupt on " << bank << pin << endl;
		}else{
			cout << "Timed out on " << bank << pin << endl;
		}
	}
}
