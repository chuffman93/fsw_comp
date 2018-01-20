/*
 * test_GroundCommunication.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alpa3119
 */


#include "test/catch.hpp"
//#include "core/ScheduleManager.h"
#include "subsystem/subsystem.h"
#include "interfaces/ACPInterface.h"
//#include "hal/GPIOManager.h"
//#include "hal/SPIManager.h"
#include <sys/types.h>
#include <sys/stat.h>

TEST_CASE("Test parse IEF and PPE file requests", "[groundcommunication]"){
	/*
	GPIOManager gpioTest("");
	SPIManager spiTest("",0,0);
	ACPInterface acpTest(gpioTest,spiTest,0,0);
	SubPowerInterface subPower(gpioTest,0,0,0,"");

	ACS acs(acpTest, subPower);
	COM com(acpTest, subPower);
	EPS eps(acpTest, subPower);
	RAD rad(acpTest, subPower);

	std::vector<SubsystemBase*> Subsystems;//= {&acs,&com,&eps,&rad};


	//GroundCommunication ground(Subsystems);

	std::vector<uint8_t> testbuff;
	testbuff.push_back(14);

	std::string testDirectory1 = "test";
	std::string testDirectory2 = "test2";

	mkdir(testDirectory1.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(testDirectory2.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	FileManager::writeToFile("test/dwlkFileTest",testbuff);
	FileManager::writeToFile("test/dwlkFileTest2",testbuff);
	FileManager::writeToFile("test/dwlkFileTest3",testbuff);
	FileManager::writeToFile("test/dwlkFileTest4",testbuff);
	FileManager::writeToFile("test/dwlkRegex_1_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex_1_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex_1_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex_1_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex_2_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex_2_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex_2_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex_2_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex_3_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex_3_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex_3_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex_3_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_1_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_1_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_1_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_1_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_2_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_2_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_2_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_2_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_3_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_3_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_3_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex2_3_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_1_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_1_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_1_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_1_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_2_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_2_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_2_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_2_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_3_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_3_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_3_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex3_3_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_1_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_1_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_1_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_1_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_2_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_2_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_2_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_2_4",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_3_1",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_3_2",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_3_3",testbuff);
	FileManager::writeToFile("test/dwlkRegex4_3_4",testbuff);
	FileManager::writeToFile("test2/testfile1",testbuff);
	FileManager::writeToFile("test2/testfile2",testbuff);
	FileManager::writeToFile("test2/testfile3",testbuff);
	FileManager::writeToFile("test2/testfile4",testbuff);
	FileManager::writeToFile("test2/testfile5",testbuff);


	//std::string systemCommand = "CMD,SYS, ";
	std::string FSWCommand = "CMD,FSW,5\n"; //clear downlink
	std::string ACSCommand = "CMD,ACS,8\n"; //test alive
	std::string COMCommand = "CMD,COM,8\n"; //test alive
	std::string EPSCommand = "CMD,EPS,8\n"; //test alive
	std::string RADCommand = "CMD,RAD,8\n"; //test alive

	std::string FSWCommand2 = "CMD,FSW,5,6\n"; //clear downlink
	std::string ACSCommand2 = "CMD,ACS,2,8\n"; //test data and alive
	std::string COMCommand2 = "CMD,COM,2,8\n"; //test data and alive
	std::string EPSCommand2 = "CMD,EPS,2,8\n"; //test data and alive
	std::string RADCommand2 = "CMD,RAD,2,8\n"; //test data and alive

	std::string DWLFile = "DWL,F,test/dwlkFileTest\n";
	std::string DWLRegex = "DWL,R,test/dwlkRegex_2\n";
	std::string DWLRegexBefore = "DWL,RB,test/dwlkRegex_2\n";
	std::string DWLRegexAfter = "DWL,RF,test/dwlkRegex_2\n";

	std::string DWLFile2 = "DWL,F,test/dwlkFileTest,test/dwlkFileTest2\n";
	std::string DWLRegex2 = "DWL,R,test/dwlkRegex_2,test/dwlkRegex2_2\n";
	std::string DWLRegexBefore2 = "DWL,RB,test/dwlkRegex_2,test/dwlkRegex2_2\n";
	std::string DWLRegexAfter2 = "DWL,RF,test/dwlkRegex_2,test/dwlkRegex2_2\n";

	std::string DFL = "DFL,test\n";

	std::string DFL2 = "DFL,test,test2\n";

	std::string DLTFile = "DLT,F,test/dwlkFileTest3\n";
	std::string DLTRegex = "DLT,R,test/dwlkRegex3_2\n";
	std::string DLTRegexBefore = "DLT,RB,test/dwlkRegex3_2\n";
	std::string DLTRegexAfter = "DLT,RF,test/dwlkRegex3_2\n";

	std::string DLTFile2 = "DLT,F,test/dwlkFileTest3,test/dwlkFileTest4\n";
	std::string DLTRegex2 = "DLT,R,test/dwlkRegex3_2,test/dwlkRegex4_2\n";
	std::string DLTRegexBefore2 = "DLT,RB,test/dwlkRegex3_2,test/dwlkRegex4_2\n";
	std::string DLTRegexAfter2 = "DLT,RF,test/dwlkRegex3_2,test/dwlkRegex4_2\n";

	std::string PPE = FSWCommand + DLTFile + ACSCommand + DLTRegex + COMCommand + DLTRegexBefore + EPSCommand + DLTRegexAfter + RADCommand;

	SECTION("Test IEF: one request per line"){
		std::string IEF = FSWCommand + DWLFile + DFL + DLTFile + ACSCommand + DWLRegex + DLTRegex + COMCommand + DWLRegexBefore + DLTRegexBefore + EPSCommand + DWLRegexAfter + DLTRegexAfter + RADCommand;

		FILE * IEFfile = fopen(IEF_PATH, "w");
		fwrite(&IEF, sizeof(std::string), sizeof(IEF),IEFfile);
		fclose(IEFfile);

		REQUIRE(!(FileManager::parseIEF(),0));
		REQUIRE(FileManager::checkExistance(DFL_PATH));
		REQUIRE(ground.DownlinkQueue.size() == 13);

	}
	SECTION("Test PPE: one request per line"){
		std::string PPE = FSWCommand + DLTFile + ACSCommand + DLTRegex + COMCommand + DLTRegexBefore + EPSCommand + DLTRegexAfter + RADCommand;

		FILE * PPEfile = fopen(PPE_PATH, "w");
		fwrite(&PPE, sizeof(std::string), sizeof(PPE),PPEfile);
		fclose(PPEfile);

		REQUIRE(!(FileManager::parsePPE(),0));
	}
	SECTION("Test IEF: two requests per line"){
		std::string IEF2 = FSWCommand2 + DWLFile2 + DFL2 + DLTFile2 + ACSCommand2 + DWLRegex2 + DLTRegex2 + COMCommand2 + DWLRegexBefore2 + DLTRegexBefore2 + EPSCommand2 + DWLRegexAfter2 + DLTRegexAfter2 + RADCommand2;

		FILE * IEFfile = fopen(IEF_PATH, "w");
		fwrite(&IEF2, sizeof(std::string), sizeof(IEF2),IEFfile);
		fclose(IEFfile);

		REQUIRE(!(FileManager::parseIEF(),0));
		REQUIRE(FileManager::checkExistance(DFL_PATH));
		REQUIRE(ground.DownlinkQueue.size() == 26);

	}
	SECTION("Test PPE: two requests per line"){
		std::string PPE2 = FSWCommand2 + DLTFile2 + ACSCommand2 + DLTRegex2 + COMCommand2 + DLTRegexBefore2 + EPSCommand2 + DLTRegexAfter2 + RADCommand2;

		FILE * PPEfile = fopen(PPE_PATH, "w");
		fwrite(&PPE2, sizeof(std::string), sizeof(PPE2),PPEfile);
		fclose(PPEfile);

		REQUIRE(!(FileManager::parseIEF(),0));
	}
	SECTION("Test execute FSW command incorrect inputs"){
		char * empty = "";
		char * incorrectSystem1 = "M";
		char * incorrectSystem2 = "FSW ";
		char * noCommand1 = "SYS";
		char * noCommand2 = "FSW";
		char * noCommand3 = "ACS";
		char * noCommand4 = "COM";
		char * noCommand5 = "EPS";
		char * noCommand6 = "RAD";
		char * incorrectCommand1 = "SYS,50000";
		char * incorrectCommand2 = "FSW,50000";
		char * incorrectCommand3 = "ACS,50000";
		char * incorrectCommand4 = "COM,50000";
		char * incorrectCommand5 = "EPS,50000";
		char * incorrectCommand6 = "RAD,50000";
		char * incorrectCommand7 = "SYS, 50000";
		char * incorrectCommand8 = "FSW, 50000";
		char * incorrectCommand9= "ACS, 50000";
		char * incorrectCommand10 = "COM, 50000";
		char * incorrectCommand11 = "EPS, 50000";
		char * incorrectCommand12 = "RAD, 50000";

		REQUIRE(!(ground.parseCommandRequest(empty),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectSystem1),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectSystem2),0));
		REQUIRE(!(ground.parseCommandRequest(noCommand1),0));
		REQUIRE(!(ground.parseCommandRequest(noCommand2),0));
		REQUIRE(!(ground.parseCommandRequest(noCommand3),0));
		REQUIRE(!(ground.parseCommandRequest(noCommand4),0));
		REQUIRE(!(ground.parseCommandRequest(noCommand5),0));
		REQUIRE(!(ground.parseCommandRequest(noCommand6),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand1),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand2),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand3),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand4),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand5),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand6),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand7),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand8),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand9),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand10),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand11),0));
		REQUIRE(!(ground.parseCommandRequest(incorrectCommand12),0));
	}
	SECTION("Test downlink nonexisting file"){
		remove(IEF_PATH);
		ground.clearDownlink();
		char * incorrectFile = "DWL," IEF_PATH;
		char * partiallyIncorrect = "DWL," PPE_PATH "," IEF_PATH;


		REQUIRE(!(ground.parseDownlinkRequest(incorrectFile),0));
		REQUIRE(ground.DownlinkQueue.empty());

		REQUIRE(!(ground.parseDownlinkRequest(partiallyIncorrect),0));
		REQUIRE(ground.DownlinkQueue.size() == 1);
	}
	SECTION("Test downlink file list incorrect"){
		remove(IEF_PATH);
		remove(DFL_PATH);
		char * incorrectFile = "DLT," IEF_PATH;
		char * partiallyIncorrect = "DLT," PPE_PATH "," IEF_PATH;

		REQUIRE(!(ground.parseDownlinkRequest(incorrectFile),0));
		REQUIRE(!(ground.parseDownlinkRequest(partiallyIncorrect),0));
		REQUIRE(!FileManager::checkExistance(PPE_PATH));
		REQUIRE(FileManager::checkExistance(DFL_PATH));
	}
	SECTION("Test delete nonexisting file"){
		remove(IEF_PATH);
		char * incorrectFile = "DLT," IEF_PATH;
		char * partiallyIncorrect = "DLT," PPE_PATH "," IEF_PATH;

		REQUIRE(!(ground.parseDownlinkRequest(incorrectFile),0));
		REQUIRE(!(ground.parseDownlinkRequest(partiallyIncorrect),0));
		REQUIRE(!FileManager::checkExistance(PPE_PATH));
	}
*/
}

TEST_CASE("DownlinkFiles"){

}


