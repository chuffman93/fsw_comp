/*
 * test_GroundCommunication.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alpa3119
 */


#include "test/catch.hpp"
#include "interfaces/ACPInterface.h"
#include "core/GroundCommunication.h"
#include "hal/GPIOManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

GPIOManager gpioTest("");
SPIManager spiTest("",0,0);


class GroundMockACPInterface: public ACPInterface{
public:
	GroundMockACPInterface(): ACPInterface(spiTest, gpioTest, 0 , 0, "") {}
	~GroundMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};




TEST_CASE("Test parse IEF and PPE file requests", "[.][groundcommunication]"){

	//Logger::setMode(MODE_PRINT);

	SubPowerInterface subPower(gpioTest,0,0,0,"");

	GroundMockACPInterface acpACS;
	GroundMockACPInterface acpCOM;
	GroundMockACPInterface acpEPS;
	GroundMockACPInterface acpRAD;

	ACS acs(acpACS, subPower);
	COM com(acpCOM, subPower);
	EPS eps(acpEPS, subPower);
	RAD rad(acpRAD, subPower);

	std::vector<SubsystemBase*> Subsystems;
	Subsystems.push_back(&acs);
	Subsystems.push_back(&com);
	Subsystems.push_back(&eps);
	Subsystems.push_back(&rad);

	GroundCommunication ground(Subsystems);

	std::vector<uint8_t> testbuff;
	testbuff.push_back(14);

	std::string testDirectory1 = HOME_DIRECTORY "/test";
	std::string testDirectory2 = HOME_DIRECTORY "/test2";

	if(!opendir(testDirectory1.c_str())){
		system(("mkdir " + testDirectory1).c_str());
	}
	if(!opendir(testDirectory2.c_str())){
		system(("mkdir " + testDirectory2).c_str());
	}

	FileManager::writeToFile(HOME_DIRECTORY "/test/dwlkFileTest",testbuff);
	FileManager::writeToFile(HOME_DIRECTORY "/test/dwlkFileTest2",testbuff);
	FileManager::writeToFile(HOME_DIRECTORY "/test2/testfile1",testbuff);
	FileManager::writeToFile(HOME_DIRECTORY "/test2/testfile2",testbuff);
	FileManager::writeToFile(HOME_DIRECTORY "/test2/testfile3",testbuff);
	FileManager::writeToFile(HOME_DIRECTORY "/test2/testfile4",testbuff);
	FileManager::writeToFile(HOME_DIRECTORY "/test2/testfile5",testbuff);



	//std::string systemCommand = "CMD,SYS, ";
	std::string FSWCommand = "CMD,FSW,5\n"; //clear downlink
	std::string ACSCommand = "CMD,ACS,2\n"; //test alive
	std::string COMCommand = "CMD,COM,2\n"; //test alive
	std::string EPSCommand = "CMD,EPS,2\n"; //test alive
	std::string RADCommand = "CMD,RAD,2\n"; //test alive

	std::string FSWCommand2 = "CMD,FSW,5,6\n"; //clear downlink
	std::string ACSCommand2 = "CMD,ACS,2,8\n"; //test data and alive
	std::string COMCommand2 = "CMD,COM,2,8\n"; //test data and alive
	std::string EPSCommand2 = "CMD,EPS,2,8\n"; //test data and alive
	std::string RADCommand2 = "CMD,RAD,2,8\n"; //test data and alive

	std::string DWLFile = "DWL,F," HOME_DIRECTORY "/test/dwlkFileTest\n";

	std::string DWLFile2 = "DWL,F," HOME_DIRECTORY "/test/dwlkFileTest," HOME_DIRECTORY "/test/dwlkFileTest2\n";

	std::string DFL = "DFL,"HOME_DIRECTORY "/test,"HOME_DIRECTORY "/test\n";

	std::string DFL2 = "DFL," HOME_DIRECTORY "/test," HOME_DIRECTORY "/test2\n";

	std::string DLTFile = "DLT,F," HOME_DIRECTORY "/test/dwlkFileTest\n";

	std::string DLTFile2 = "DLT,F," HOME_DIRECTORY "/test/dwlkFileTest," HOME_DIRECTORY "/test/dwlkFileTest1\n";

	SECTION("Ground Communication: Test commands"){
		std::string commandIEF = FSWCommand2 + ACSCommand2 + COMCommand2 + EPSCommand2 + RADCommand2;

		std::ofstream IEFfile;
		IEFfile.open(IEF_PATH);
		IEFfile << commandIEF;
		IEFfile.close();

		ground.parseIEF();

		//TODO: test will not pass when using the 8th opcode
		REQUIRE(acpACS.sentOpcodes.end() != std::find(acpACS.sentOpcodes.begin(), acpACS.sentOpcodes.end(), OP_TESTDATA));
		REQUIRE(acpCOM.sentOpcodes.end() != std::find(acpCOM.sentOpcodes.begin(), acpCOM.sentOpcodes.end(), OP_TESTDATA));
		REQUIRE(acpEPS.sentOpcodes.end() != std::find(acpEPS.sentOpcodes.begin(), acpEPS.sentOpcodes.end(), OP_TESTDATA));
		REQUIRE(acpRAD.sentOpcodes.end() != std::find(acpRAD.sentOpcodes.begin(), acpRAD.sentOpcodes.end(), OP_TESTDATA));
		REQUIRE(acpACS.sentOpcodes.end() != std::find(acpACS.sentOpcodes.begin(), acpACS.sentOpcodes.end(), OP_TESTALIVE));
		REQUIRE(acpCOM.sentOpcodes.end() != std::find(acpCOM.sentOpcodes.begin(), acpCOM.sentOpcodes.end(), OP_TESTALIVE));
		REQUIRE(acpEPS.sentOpcodes.end() != std::find(acpEPS.sentOpcodes.begin(), acpEPS.sentOpcodes.end(), OP_TESTALIVE));
		REQUIRE(acpRAD.sentOpcodes.end() != std::find(acpRAD.sentOpcodes.begin(), acpRAD.sentOpcodes.end(), OP_TESTALIVE));
	}

	SECTION("Ground Communication: Test downlink file list"){
		remove(DFL_PATH);

		std::string fileListIEF = DFL;// + DFL2;

		std::ofstream IEFfile;
		IEFfile.open(IEF_PATH);
		IEFfile << fileListIEF;
		IEFfile.close();

		ground.parseIEF();

		//TODO: DFL2 does not work
		REQUIRE(FileManager::checkExistance(DFL_PATH));


	}

	SECTION("Ground Communication: Test downlink"){


		std::string commandIEF = DWLFile + DWLFile2;

		std::ofstream IEFfile;
		IEFfile.open(IEF_PATH);
		IEFfile << commandIEF;
		IEFfile.close();

		ground.parseIEF();

		REQUIRE(ground.DownlinkQueue.size() == 3);


		REQUIRE(ground.DownlinkQueue.front() ==  HOME_DIRECTORY "/test/dwlkFileTest");
		ground.DownlinkQueue.pop();
		REQUIRE(ground.DownlinkQueue.front() ==  HOME_DIRECTORY "/test/dwlkFileTest");
		ground.DownlinkQueue.pop();
		REQUIRE(ground.DownlinkQueue.front() == HOME_DIRECTORY "/test/dwlkFileTest2");


	}

	SECTION("Ground Communication: Test deletion"){
		std::string deleteIEF = DLTFile2;

		std::ofstream IEFfile;
		IEFfile.open(IEF_PATH);
		IEFfile << deleteIEF;
		IEFfile.close();

		ground.parseIEF();

		REQUIRE(!FileManager::checkExistance(HOME_DIRECTORY "/test/dwlkFileTest"));
		//REQUIRE(!FileManager::checkExistance("test/dwlkFileTest2"));
	}

	Logger::setMode(MODE_NOTHING);
}



