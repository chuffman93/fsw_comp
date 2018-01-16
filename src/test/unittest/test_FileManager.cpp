
#include "test/catch.hpp"
#include "core/FileManager.h"
#include "core/ScheduleManager.h"
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
	std::vector<uint8_t> dummyVec;
	dummyVec.assign(1,12);

	remove(dummyFile.c_str());

	FILE * dummy = fopen(dummyFile.c_str(),"w");
	fclose(dummy);

	SECTION("TEST NO FILE PATH"){
		dummyFile = "";
		FileManager::deleteFile(dummyFile);
	}
	SECTION("TEST INCORRECT FILE PATH"){
		dummyFile = "dumdum";
		remove(dummyFile.c_str());
		FileManager::deleteFile(dummyFile);
	}

	SECTION("TEST CORRECT FILE PATH"){
		FileManager::deleteFile(dummyFile);
		REQUIRE(access(dummyFile.c_str(), F_OK) == -1);

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

	// cout << newDummy << endl;
	// TODO: Maybe create a REQUIRE case to pass

}



TEST_CASE("FILEMANAGER: DELETE WITH REGEX", "[filemanager]"){
	std::string deleteSet = "/home/EPS_92";

}

TEST_CASE("FILEMANAGER: GET FILE LIST","[filemanager]"){
	std::string dir = "/home";
	FileManager::getFilesList(dir);

	std::string newF = DFL_PATH;
	REQUIRE(access((char*)newF.c_str(),F_OK) == 0);

}

TEST_CASE("FILEMANAGER: PACKAGE FILES","[filemanager]"){
	std::string packageSet = "/home/chuffman93/Desktop/attachments/PackageTest/";
	std::string regex = "EPS_92";
	std::string dest = "/home/EPS_92.tar";
	REQUIRE((FileManager::packageFiles((char*)dest.c_str(),(char*)packageSet.c_str(),(char*)regex.c_str()))==0);
}





