/*
 * test_RAD.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: chuffman93
 */


#include "test/catch.hpp"
#include <subsystem/RAD.h>
#include <iterator>
#include <iostream>
#include "hal/GPIOManager.h"

SPIManager dummySR("", 0, 0);
GPIOManager dummyGR("");

class RADMockACPInterface: public ACPInterface{
public:
	RADMockACPInterface(): ACPInterface(dummySR, dummyGR, 0, 0){}
	~RADMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};


TEST_CASE("SPLIT DATA","[PLD]"){
	RADMockACPInterface acp;
	SubPowerInterface subPower(dummyGR, 0, 0, 0, "");
	RAD rad(acp,subPower);

	rad.commandCollectionBegin();
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));



	std::vector<uint8_t> buff;
	buff.assign(1000000,1);


	std::ofstream in("/home/Random");
	for(int i = 0; i < buff.size(); i++){
		in << buff[i];
	}




	rad.commandCollectionEnd();




}
