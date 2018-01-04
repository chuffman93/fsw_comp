
#include "test/catch.hpp"
#include "core/FileManager.h"
#include <stdio.h>
#include <iostream>

using namespace std;

std::vector<uint8_t> dummyVec;
const std::string & file = "dummy.txt";
class MockFileInterface: public FileInterface{
public:
	InterfaceOperation getOperation(){
		InterfaceOperation io(FIO_READ, file, dummyVec);
		return io;
	}
};

TEST_CASE("FILEMANAGER: TEST READ FROM FILE", "[filemanager]"){
	remove("dummy.txt");
	FileManager fm;
	MockFileInterface mfi;
	fm.registerInterface(&mfi);
	remove("dummy.txt");

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

TEST_CASE("FILEMANAGER: TEST WRITE TO FILE", "[filemanager]"){
	remove("dummy.txt");
	FileManager fm;
	MockFileInterface mfi;
	fm.registerInterface(&mfi);
	InterfaceOperation write = mfi.getOperation();
	write.type = FIO_WRITE;
	write.filePath = "dummy";
	dummyVec.assign(1,12);

	SECTION("TEST NO FILE PATH"){
		write.data = dummyVec;
		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST EMPTY MESSAGE"){
		fm.spinInterfaces();
		REQUIRE(dummyVec.size() == 0);
	}
	SECTION("TEST NONEMPTY MESSAGE"){
		write.data = dummyVec;
		fm.spinInterfaces();

		write.type = FIO_READ;
		fm.spinInterfaces();

		REQUIRE(dummyVec[0]== 12);
	}

}

TEST_CASE("FILEMANAGER: TEST DELETE FILE", "[filemanager]"){
	FileManager fm;
	MockFileInterface mfi;
	fm.registerInterface(&mfi);
	InterfaceOperation write = mfi.getOperation();
	write.type = FIO_DELETE;

	FILE * dummyFile = fopen(write.filePath.c_str(),"w");
	fclose(dummyFile);

	SECTION("TEST NO FILE PATH"){
		write.filePath.clear();
		fm.spinInterfaces();
		//REQUIRE(write.filePath == "incorrect");
	}
	SECTION("TEST INCORRECT FILE PATH"){
		remove(write.filePath.c_str());
		fm.spinInterfaces();
		//REQUIRE(write.filePath == "incorrect");
	}

	SECTION("TEST CORRECT FILE PATH"){
		fm.spinInterfaces();
		//REQUIRE(write.filePath != "incorrect");
	}
}


