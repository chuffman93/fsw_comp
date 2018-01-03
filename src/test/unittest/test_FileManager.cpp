
#include "test/catch.hpp"
#include "core/FileManager.h"
#include <stdio.h>

std::vector<uint8_t> dummyVec;

class MockFileInterface: public FileInterface{
public:
	InterfaceOperation getOperation(){
		InterfaceOperation io(FIO_READ, "dummy.txt", dummyVec);
		return io;
	}
};

TEST_CASE("FILEMANAGER: TEST READ FROM FILE", "[filemanager]"){
	FileManager fm;
	MockFileInterface mfi;
	fm.registerInterface(&mfi);

	SECTION("TEST NO FILE PATH"){
		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST EMPTY FILE"){
		//create blank file
		FILE * dummyFile = fopen("dummy.txt","w");
		fclose(dummyFile);
		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY FILE"){
		FILE * dummyFile = fopen("dummy.txt","w");
		std::string text = "Hello World";
		fwrite(text.c_str(),sizeof(char), text.length(), dummyFile);
		fclose(dummyFile);
		fm.spinInterfaces();
		std::string textRead(dummyVec.begin(), dummyVec.end());
		REQUIRE(textRead == text);
	}
	//put data into the file
	//read data from file and place in buffer
}
