
#include "test/catch.hpp"
#include "core/FileManager.h"
#include <stdio.h>
#include <iostream>

using namespace std;


TEST_CASE("FILEMANAGER: TEST READ FROM FILE", "[filemanager]"){
	std::string dummyFile = "dummy";
	std::vector<uint8_t> dummyVec;

	remove(dummyFile.c_str());

	FileManager fm;


	SECTION("TEST NO FILE PATH"){
		dummyVec = fm.readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST EMPTY FILE"){
		//create blank file
		FILE * dummy = fopen(dummyFile.c_str(),"w");
		fclose(dummy);
		dummyVec = fm.readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY FILE"){
		FILE * dummy = fopen(dummyFile.c_str(),"w");
		std::string text = "Hello World";
		fwrite(text.c_str(),sizeof(char), text.length(), dummy);
		fclose(dummy);
		dummyVec = fm.readFromFile(dummyFile);
		std::string textRead(dummyVec.begin(), dummyVec.end());
		REQUIRE(textRead == text);
	}
}

TEST_CASE("FILEMANAGER: TEST WRITE TO FILE", "[filemanager]"){
	std::string dummyFile = "dummy";
	std::vector<uint8_t> dummyVec;
	dummyVec.assign(1,12);

	remove(dummyFile.c_str());

	FileManager fm;


	SECTION("TEST NO FILE PATH"){
		dummyFile = "";
		fm.writeToFile(dummyFile.c_str(), dummyVec);
	}
	SECTION("TEST EMPTY MESSAGE"){
		dummyVec.clear();

		fm.writeToFile(dummyFile.c_str(), dummyVec);

		REQUIRE(access(dummyFile.c_str(),F_OK));

		dummyVec = fm.readFromFile(dummyFile);
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY MESSAGE"){

		fm.writeToFile(dummyFile.c_str(), dummyVec);

		REQUIRE(access(dummyFile.c_str(),F_OK) == 0);

		std::vector<uint8_t> dummyVecOut = fm.readFromFile(dummyFile);
		REQUIRE(dummyVec == dummyVec);
	}

}

TEST_CASE("FILEMANAGER: TEST DELETE FILE", "[filemanager]"){
	std::string dummyFile = "dummy";
	std::vector<uint8_t> dummyVec;
	dummyVec.assign(1,12);

	remove(dummyFile.c_str());

	FileManager fm;

	FILE * dummy = fopen(dummyFile.c_str(),"w");
	fclose(dummy);

	SECTION("TEST NO FILE PATH"){
		dummyFile = "";
		fm.deleteFile(dummyFile);
	}
	SECTION("TEST INCORRECT FILE PATH"){
		dummyFile = "dumdum";
		remove(dummyFile.c_str());
		fm.deleteFile(dummyFile);
	}

	SECTION("TEST CORRECT FILE PATH"){
		fm.deleteFile(dummyFile);
		REQUIRE(access(dummyFile.c_str(), F_OK) == -1);

	}
}

TEST_CASE("FILEMANAGER: TEST CHECK EXISTANCE", "[filemanager]"){
	std::string dummyFile = "dummy";

	remove(dummyFile.c_str());

	FileManager fm;

	SECTION("TEST NONEXISTANT FILE"){
		REQUIRE(fm.checkExistance(dummyFile) == false);
	}
	SECTION("TEST EXISTING FILE"){

		FILE * dummy = fopen(dummyFile.c_str(),"w");
		fclose(dummy);
		REQUIRE(fm.checkExistance(dummyFile) == true);
	}
}

TEST_CASE("FILEMANAGER: TEST MOVE FILE", "[filemanager]"){

	std::string dummyFile = "dummy";
	std::string newdummyFile = "dummmmy";

	remove(dummyFile.c_str());
	FILE * dummy = fopen(dummyFile.c_str(),"w");
	fclose(dummy);

	FileManager fm;

	SECTION("TEST INCORRECT FILEPATH"){
		remove(dummyFile.c_str());
		fm.moveFile(dummyFile, newdummyFile);
		REQUIRE(access(newdummyFile.c_str(),F_OK) == -1);
	}
	SECTION("TEST INCORRECT NEW FILEPATH"){
		newdummyFile = "";
		fm.moveFile(dummyFile, newdummyFile);
		REQUIRE(access(dummyFile.c_str(),F_OK) == 0);

	}
	SECTION("TEST CORRECT FILE PATH AND NEW FILE PATH "){
		fm.moveFile(dummyFile, newdummyFile);
		REQUIRE(access(newdummyFile.c_str(),F_OK) == 0);
	}

	remove(newdummyFile.c_str());

}

TEST_CASE("FILEMANAGER: TEST CREATE FILE NAME", "[filemanager]" ){
	std::string dummyFile = "dummy";

	FileManager fm;
	std::string newDummy = fm.createFileName(dummyFile);


}


