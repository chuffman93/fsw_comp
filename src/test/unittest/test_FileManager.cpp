
#include "test/catch.hpp"
#include "core/FileManager.h"
#include <stdio.h>
#include <iostream>

using namespace std;

std::vector<uint8_t> dummyVec;
std::string file = "dummy";
FileIOType type = FIO_READ;

class MockFileInterface: public FileInterface{
public:
	InterfaceOperation getOperation(){
		InterfaceOperation io(type, file, dummyVec);
		return io;
	}
};

TEST_CASE("FILEMANAGER: TEST READ FROM FILE", "[filemanager]"){
	remove("dummy");
	FileManager fm;
	MockFileInterface mfi;
	fm.registerInterface(&mfi);
	remove("dummy");

	SECTION("TEST NO FILE PATH"){
		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST EMPTY FILE"){
		//create blank file
		FILE * dummyFile = fopen("dummy","w");
		fclose(dummyFile);
		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY FILE"){
		FILE * dummyFile = fopen("dummy","w");
		std::string text = "Hello World";
		fwrite(text.c_str(),sizeof(char), text.length(), dummyFile);
		fclose(dummyFile);
		fm.spinInterfaces();
		std::string textRead(dummyVec.begin(), dummyVec.end());
		REQUIRE(textRead == text);
	}
}

TEST_CASE("FILEMANAGER: TEST WRITE TO FILE", "[filemanager]"){
	remove("dummy");
	type = FIO_WRITE;
	dummyVec.assign(1,12);
	file = "dummy";

	SECTION("TEST NO FILE PATH"){
		file = "";

		FileManager fm;
		MockFileInterface mfi;
		fm.registerInterface(&mfi);

		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST EMPTY MESSAGE"){
		dummyVec.clear();

		FileManager fm;
		MockFileInterface mfi;
		fm.registerInterface(&mfi);

		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY MESSAGE"){
		FileManager fm;
		MockFileInterface mfi;
		fm.registerInterface(&mfi);
		InterfaceOperation write = mfi.getOperation();
		write.filePath = "dummy";
		dummyVec.assign(1,12);

		write.data = dummyVec;
		fm.spinInterfaces();

		write.type = FIO_READ;
		fm.spinInterfaces();

		REQUIRE(dummyVec[0]== 12);
	}

}

TEST_CASE("FILEMANAGER: TEST DELETE FILE", "[filemanager]"){
	type = FIO_DELETE;
	file = "dummy";

	FILE * dummyFile = fopen(file.c_str(),"w");
	fclose(dummyFile);

	SECTION("TEST NO FILE PATH"){
		file = "";

		FileManager fm;
		MockFileInterface mfi;
		fm.registerInterface(&mfi);

		fm.spinInterfaces();
	}
	SECTION("TEST INCORRECT FILE PATH"){
		FileManager fm;
		MockFileInterface mfi;
		fm.registerInterface(&mfi);

		remove(file.c_str());
		fm.spinInterfaces();
	}

	SECTION("TEST CORRECT FILE PATH"){
		FileManager fm;
		MockFileInterface mfi;
		fm.registerInterface(&mfi);

		fm.spinInterfaces();
		FILE * dumFile = fopen(file.c_str(),"r");
		REQUIRE(dumFile == NULL);
	}
}


