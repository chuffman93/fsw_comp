/*
 * test_UARTManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "hal/UARTManager.h"
#include "test/catch.hpp"
#include "test/testmacros.h"
#include "util/Logger.h"
#include <iostream>

using namespace std;

//TODO: Verify that UART read is blocking now
TEST_CASE("Test UART Manager read", "[.][hardware][uart][read]"){
	Logger::setMode(MODE_PRINT);
	cout << "---------------UART read test----------------" << endl;
	UARTManager uart("/dev/ttyS2");
	uart.initialize();
	PROMPT("Send a string over the UART Line, then continue...");
	vector<uint8_t> bytesreceived = uart.readData(10);
	string received(bytesreceived.begin(), bytesreceived.end());
	cout <<"Received: " << received << endl;
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("Test UART Manager write", "[.][hardware][uart][write]"){
	cout << "---------------UART write test----------------" << endl;
	UARTManager uart("/dev/ttyS2");
	uart.initialize();
	PROMPT("Ready to send string over UART...");
	string teststr = "Hello World!";
	std::vector<uint8_t> testbytes(teststr.begin(), teststr.end());
	uart.writeData(testbytes);
	cout <<"Sent: " << teststr<< endl;
}


