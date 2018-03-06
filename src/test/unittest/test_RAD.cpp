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
	RADMockACPInterface(): ACPInterface(dummySR, dummyGR, 0, 0, ""){}
	~RADMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		ret.opcode = packet.opcode;
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};


TEST_CASE("TEST INITIALIZATION ROUTINE","[PLD]"){
	SPIManager dummySR("", 0, 0);
	GPIOManager dummyGR("");
	RADMockACPInterface acp;
	SubPowerInterface subPower(dummyGR, 0, 0, 0, "");
	RAD rad(acp,subPower);

	REQUIRE(rad.commandCollectionBegin());
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));

}

TEST_CASE("SPLIT DATA","[.][PLD][splt]"){
	SPIManager dummySR("", 0, 0);
	GPIOManager dummyGR("");
	RADMockACPInterface acp;
	SubPowerInterface subPower(dummyGR, 0, 0, 0, "");
	RAD rad(acp,subPower);
	FSWMode T1 = Trans_BusToPayload;
	FSWMode T2 = Trans_PayloadToBus;
	rad.handleMode(T1);
	std::vector<uint8_t> buff;
	buff.assign(1000000,1);


	char mockDataFile[100];
	sprintf(mockDataFile,RAD_FILE_PATH"_%d_%d_",FileManager::GetReboot(),getCurrentTime());
	std::fstream in(mockDataFile, std::ofstream::out | std::ofstream::binary);
	for(std::vector<uint8_t>::iterator i = buff.begin(); i != buff.end(); i++){
		in << *i;
	}
	in.close();
	char mock2[100];
	sprintf(mock2,RAD_FILE_PATH"_%d_%d_003.tar.gz",FileManager::GetReboot(),getCurrentTime());
	REQUIRE(access(mockDataFile,F_OK) == 0);
	REQUIRE(access(mock2,F_OK) == -1);
	rad.handleMode(T2);
	REQUIRE(access(mockDataFile,F_OK) == -1);
	REQUIRE(access(mock2,F_OK) == 0);
	system("rm " RAD_FILE_PATH "_*");
}


