
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
		//TODO: fix this so it can write/read mulitple bytes of different sizes including serialization
		uint8_t mode = 14;
		std::vector<uint8_t> buffer;
		buffer.clear();
		buffer.push_back(mode);


		int fileID = open(dummyFile.c_str(),O_CREAT | O_WRONLY);
		write(fileID,&buffer[0],sizeof(buffer));
		close(fileID);

		std::vector<uint8_t> dummyVec = FileManager::readFromFile(dummyFile);
		// cout << dummyVec.size() << endl;


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
		FileManager::writeToFile(dummyFile.c_str(), dummyVec);
	}
	SECTION("TEST EMPTY MESSAGE"){
		dummyVec.clear();

		FileManager::writeToFile(dummyFile.c_str(), dummyVec);

		REQUIRE(access(dummyFile.c_str(),F_OK));

		dummyVec = FileManager::readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY MESSAGE"){

		FileManager::writeToFile(dummyFile.c_str(), dummyVec);

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

	ScheduleManager sch;
	std::string newDummy = FileManager::createFileName(dummyFile);
	//cout << newDummy << endl;


}



