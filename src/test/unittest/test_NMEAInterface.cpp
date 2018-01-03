/*
 * test_NMEAInterface.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: cyborg9
 */
#include "test/catch.hpp"
#include "interfaces/NMEAInterface.h"
#include <queue>

class MockUARTManager: public UARTManager{
public:
	MockUARTManager(): UARTManager(""){}

	void initialize(){}
	uint8_t readData(){
		uint8_t d = queueddata.front();
		queueddata.pop();
		return d;
	}
	void writeData(std::vector<uint8_t> data){}
	std::queue<uint8_t> queueddata;
};

TEST_CASE("Test NMEAInterface receive", "[interfaces][NMEAInterface]"){
	std::string teststr = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	std::string expectedret(++teststr.begin(), teststr.end());
	MockUARTManager uart;
	NMEAInterface nm(uart);
	SECTION("Test Nominal"){
		for(std::string::iterator i = teststr.begin(); i != teststr.end(); i++){
			uart.queueddata.push(*i);
		}
		REQUIRE(nm.getString() == expectedret);
	}
	SECTION("Test padding on front"){
		teststr = "asdf" + teststr;
		for(std::string::iterator i = teststr.begin(); i != teststr.end(); i++){
			uart.queueddata.push(*i);
		}
		REQUIRE(nm.getString() == expectedret);
	}
	SECTION("Test padding on end"){
		teststr = teststr + "asdf";
		for(std::string::iterator i = teststr.begin(); i != teststr.end(); i++){
			uart.queueddata.push(*i);
		}
		REQUIRE(nm.getString() == expectedret);
	}
}




