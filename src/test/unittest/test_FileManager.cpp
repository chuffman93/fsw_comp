
#include "test/catch.hpp"
#include "core/FileManager.h"
#include "core/ScheduleManager.h"
#include "interfaces/ExternalProcess.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

TEST_CASE("FILEMANAGER: TEST READ FROM FILE", "[filemanager]"){
	std::string dummyFile = "dummy";

	remove(dummyFile.c_str());


	SECTION("TEST NO FILE PATH"){
		std::vector<uint8_t> dummyVec = FileManager::readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST EMPTY FILE"){
		//create blank file
		FILE * dummy = fopen(dummyFile.c_str(),"w");
		fclose(dummy);
		std::vector<uint8_t> dummyVec = FileManager::readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY FILE"){
		uint8_t mode = 14;
		std::vector<uint8_t> buffer;
		buffer.clear();
		buffer.push_back(mode);


		int fileID = open(dummyFile.c_str(),O_CREAT | O_WRONLY);
		write(fileID,&buffer[0],sizeof(buffer));
		close(fileID);

		std::vector<uint8_t> dummyVec = FileManager::readFromFile(dummyFile);


		REQUIRE(dummyVec[0] == 14);
	}
}

TEST_CASE("FILEMANAGER: TEST WRITE TO FILE", "[filemanager]"){
	std::string dummyFile = "dummy";
	std::vector<uint8_t> dummyVec;
	dummyVec.assign(1,12);

	remove(dummyFile.c_str());


	SECTION("TEST NO FILE PATH"){
		dummyFile = "";
		FileManager::writeToFile(dummyFile, dummyVec);
	}
	SECTION("TEST EMPTY MESSAGE"){
		dummyVec.clear();

		FileManager::writeToFile(dummyFile, dummyVec);

		REQUIRE(access(dummyFile.c_str(),F_OK));

		dummyVec = FileManager::readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY MESSAGE"){

		FileManager::writeToFile(dummyFile, dummyVec);

		REQUIRE(access(dummyFile.c_str(),F_OK) == 0);

		std::vector<uint8_t> dummyVecOut = FileManager::readFromFile(dummyFile);
		REQUIRE(dummyVec == dummyVec);
	}

}

TEST_CASE("FILEMANAGER: TEST DELETE FILE", "[filemanager]"){
	std::string dummyFile = "dummy";
	std::string dummyFolder = "/home/DummyFolder/dummdumm.txt";
	std::vector<uint8_t> dummyVec;
	dummyVec.assign(1,12);

	mkdir("/home/DummyFolder",S_IRWXU | S_IRWXG | S_IRWXO);


	FILE * dummy = fopen(dummyFile.c_str(),"w");
	fclose(dummy);


	FILE * dummyf = fopen(dummyFolder.c_str(),"w");
	fclose(dummyf);


	SECTION("TEST NO FILE PATH"){
		dummyFile = "";
		FileManager::deleteFile(dummyFile);
	}
	SECTION("TEST INCORRECT FILE PATH"){
		dummyFile = "dumdum";
		remove(dummyFile.c_str());
		REQUIRE(!(FileManager::deleteFile(dummyFile),0));
	}

	SECTION("TEST CORRECT FILE PATH"){
		FileManager::deleteFile(dummyFile);
		REQUIRE(access(dummyFile.c_str(), F_OK) == -1);

	}

	SECTION("TEST FOLDER DELETION"){
		REQUIRE(access(dummyFolder.c_str(),F_OK) == 0);
		FileManager::deleteFile("/home/DummyFolder");
		sleep(2);
		REQUIRE(access(dummyFolder.c_str(),F_OK) == -1);
	}
}

TEST_CASE("FILEMANAGER: TEST CHECK EXISTANCE", "[filemanager]"){
	std::string dummyFile = "dummy";

	remove(dummyFile.c_str());

	SECTION("TEST NONEXISTANT FILE"){
		REQUIRE(FileManager::checkExistance(dummyFile) == false);
	}
	SECTION("TEST EXISTING FILE"){

		FILE * dummy = fopen(dummyFile.c_str(),"w");
		fclose(dummy);
		REQUIRE(FileManager::checkExistance(dummyFile) == true);
	}
}

TEST_CASE("FILEMANAGER: TEST MOVE FILE", "[filemanager]"){

	std::string dummyFile = "dummy";
	std::string newdummyFile = "dummmmy";

	remove(dummyFile.c_str());
	FILE * dummy = fopen(dummyFile.c_str(),"w");
	fclose(dummy);


	SECTION("TEST INCORRECT FILEPATH"){
		remove(dummyFile.c_str());
		FileManager::moveFile(dummyFile, newdummyFile);
		REQUIRE(access(newdummyFile.c_str(),F_OK) == -1);
	}
	SECTION("TEST INCORRECT NEW FILEPATH"){
		newdummyFile = "";
		FileManager::moveFile(dummyFile, newdummyFile);
		REQUIRE(access(dummyFile.c_str(),F_OK) == 0);

	}
	SECTION("TEST CORRECT FILE PATH AND NEW FILE PATH "){
		FileManager::moveFile(dummyFile, newdummyFile);
		REQUIRE(access(newdummyFile.c_str(),F_OK) == 0);
	}

	remove(newdummyFile.c_str());

}

TEST_CASE("FILEMANAGER: TEST CREATE FILE NAME", "[filemanager]" ){
	std::string dummyFile = "dummy";
	std::string newDummy = FileManager::createFileName(dummyFile);

	std::ifstream rebootFile(REBOOT_FILE);
	int intCount;
	rebootFile >> intCount;
	rebootFile.close();

	//convert boot count to string
	stringstream strCount;
	strCount << intCount;
	std:: string RebootCount = strCount.str();

	//get current time
	uint32_t currentTime = getCurrentTime();
	stringstream ss;
	ss << currentTime;
	std::string time = ss.str();

	std::string filePath = dummyFile + "_" + RebootCount + "_" + time;
	REQUIRE(filePath == newDummy);

}

TEST_CASE("FILEMANAGE: TEST COPY FILE", "[filemanager]"){
	std::string filename = "test";
	std::string copy = "testcopy";
	std::vector<uint8_t> buffer;
	buffer.push_back(1);

	SECTION("Test existing file"){
		FileManager::writeToFile(filename,buffer);
		FileManager::copyFile(filename,copy);
		REQUIRE(FileManager::checkExistance(filename));
		REQUIRE(FileManager::checkExistance(copy));
		FileManager::deleteFile(filename);
		FileManager::deleteFile(copy);
	}
	SECTION("Test nonexisting file"){
		FileManager::copyFile(filename,copy);
		REQUIRE(!FileManager::checkExistance(filename));
		REQUIRE(!FileManager::checkExistance(copy));

	}
}



TEST_CASE("FILEMANAGER: GET FILE LIST","[filemanager]"){
	std::string dir = "/home";
	FileManager::getFilesList(dir);
	std::string newF = DFL_PATH;
	REQUIRE(access((char*)newF.c_str(),F_OK) == 0);

}


TEST_CASE("FILEMANAGER: Manage Regex","[filemanager]"){

	char fileName[100];
	for(int i = 0; i < 40; i++){
		sprintf(fileName,"/home/EPS_5_%d.txt",i);
		std::ofstream file(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_4_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_3_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_2_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_1_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_0_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_38_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_39_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_40_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_41_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
		sprintf(fileName,"/home/EPS_42_%d.txt",i);
		file.open(fileName);
		file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
		file.close();
		memset(fileName,0,100);
	}
	std::ofstream file("/home/EPS_6_18928.txt");
	file << "THIS IS A TEST FILE\nUSED TO TEST REGEX DELETION\n";
	file.close();



	SECTION("HANDLE PACKAGING"){
		std::string filePath = "/home/EPS_5";
		std::vector<std::string> Files;

		// Within the regex epoch
		std::string R = "R";
		Files.push_back(filePath+".tar");
		REQUIRE(FileManager::packageFiles((char*)filePath.c_str(),R).front() == Files.front());
		Files.clear();
		for(int i = 5; i >= 0; i--){
			std::ostringstream oss;
			oss << i;
			std::string fileP = "/home/EPS_"+oss.str();
			Files.push_back(fileP+".tar");

		}


		// Before regex epoch
		R = "RB";
		std::vector<std::string> test = FileManager::packageFiles((char*)filePath.c_str(),R);
		REQUIRE(test[0][0] == Files[0][0]);
		REQUIRE(test[0][1] == Files[0][1]);
		REQUIRE(test[0][2] == Files[0][2]);
		REQUIRE(test[0][3] == Files[0][3]);
		REQUIRE(test[0][4] == Files[0][4]);

		Files.clear();
		for(int i = 42; i >= 38; i--){
			std::ostringstream oss;
			oss << i;
			std::string fileP = "/home/EPS_"+oss.str();
			Files.push_back(fileP+".tar");

		}

		R = "RA";
		filePath = "/home/EPS_38";
		test = FileManager::packageFiles((char*)filePath.c_str(),R);
		REQUIRE(test[0][0] == Files[0][0]);
		REQUIRE(test[0][1] == Files[0][1]);
		REQUIRE(test[0][2] == Files[0][2]);
		REQUIRE(test[0][3] == Files[0][3]);
		REQUIRE(test[0][4] == Files[0][4]);

	}

	SECTION("HANDLE DELETION"){
		std::string filePath = "/home/EPS_5";
		std::string R = "R";
		REQUIRE(access("/home/EPS_5_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_3_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_1_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_41_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_38_1.txt",F_OK) == 0);
		REQUIRE((FileManager::regexDelete(filePath,R) == 0));
		REQUIRE(access("/home/EPS_5_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_3_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_41_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_38_1.txt",F_OK) == 0);
		std::vector<std::string> Files;
		for(int i = 5; i >= 0; i--){
			std::ostringstream oss;
			oss << i;
			std::string fileP = "/home/EPS_"+oss.str();
			Files.push_back(fileP+".tar");
		}

		R = "RB";
		REQUIRE(access("/home/EPS_5_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_3_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_1_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_41_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_38_1.txt",F_OK) == 0);
		REQUIRE((FileManager::regexDelete(filePath,R) == 0));
		REQUIRE(access("/home/EPS_5_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_3_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_1_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_41_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_38_1.txt",F_OK) == 0);

		filePath = "/home/EPS_38";
		R = "RA";
		REQUIRE(access("/home/EPS_5_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_3_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_1_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_41_1.txt",F_OK) == 0);
		REQUIRE(access("/home/EPS_38_1.txt",F_OK) == 0);
		REQUIRE((FileManager::regexDelete(filePath,R) == 0));
		REQUIRE(access("/home/EPS_5_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_3_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_1_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_41_1.txt",F_OK) == -1);
		REQUIRE(access("/home/EPS_38_1.txt",F_OK) == -1);
	}

	char argv[100];
	sprintf(argv,"rm -r /home/EPS_*");
	FILE * fd;
	fd = popen(argv, "r");
	pclose(fd);
}


TEST_CASE("FILEMANAGER: GET DOWNLINK FILE", "[filemanager]"){

}





