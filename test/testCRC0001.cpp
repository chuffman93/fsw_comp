#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "core/Factory.h"
#include "stdlib.h"
#include <stdio.h>


using namespace std;
using namespace rel_ops;
//using namespace AllStar::HAL;
using namespace AllStar::Core;

//ensure that the function correctly calculates the crc
//the file "test1" should be in the home directory
TEST(DISABLED_TestCRC, correctCRC) {
	char filename[] = "test1"; //File contains: "Test File Text7\n"

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	//Note the NL line feed at the end of the file
	ASSERT_EQ(0x77ABD742,fileHandler->crcCheck(filename));
}

//ensure that the function catches a corrupted file
//the file "test2" should be in the home directory
TEST(DISABLED_TestCRC, corruptedFile){
	char filename[] = "test2"; //File should contain: "Text for test file 2\n"
	//File actually contains: "Text for test fjle 2\n"

	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	EXPECT_EQ(0xDA3050DC,fileHandler->crcCheck(filename)); //crc for corrupted file == calculated
	ASSERT_NE(0x5CA42272,fileHandler->crcCheck(filename)); //crc for correct file != calculated

}
