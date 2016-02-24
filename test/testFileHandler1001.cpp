#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "servers/DispatchStdTasks.h"
#include "servers/ACSStdTasks.h"
#include "core/Factory.h"
#include "stdlib.h"
#include <stdio.h>
#include "core/DataMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace rel_ops;
//using namespace Phoenix::HAL;
using namespace Phoenix::Core;

TEST(DISABLED_TestFileHandler, exist) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	//ASSERT_FALSE(fileHandler->FileExists("noooope.txt"));
	//ASSERT_TRUE(fileHandler->FileExists("CMD_List_1.dat")); //this file should be in home directory
	//EXPECT_EQ(fileHandler->fileSize("CMD_List_1.dat"),0);
}

TEST(DISABLED_TestFileHandler, sdNavRead) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	//ASSERT_TRUE(fileHandler->FileExists("/media/sdMount/test.c"));
	string file = "/media/sdMount/test.c";
	uint32_t size = fileHandler->fileSize(file.c_str());
	cout << "File Size: " << size << endl;

	ASSERT_EQ(size, 6);
	//ASSERT_TRUE(fileHandler->FileOpens("/media/sdMount/test.c")); //This function deletes the file contents!

	//const char filename = "/media/sdMount/test.c";
	//uint8 * buf = fileHandler->ReadFile(*filename,*size);
}

TEST(TestFileHandler, appendDebug) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	ErrorMessage * msg = new ErrorMessage(ACS_HS_FAILURE);
	ReturnMessage * ret = new ReturnMessage(msg, false);
	MultiDataMessage * dataMessage = dynamic_cast<MultiDataMessage *> (ret->GetMessagePtr());
	ASSERT_TRUE(fileHandler->Append(SUBSYSTEM_ACS, ACS_HS_FAILURE, (* dataMessage)));

}

TEST(TestFileHandler, testRead) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	size_t readSize = 0;
	uint8 * readBuffer;
	char * testFile = (char *)"/media/sdMount/adam.txt";

	readBuffer = fileHandler->ReadFile(testFile, &readSize);
	ASSERT_TRUE(readBuffer != NULL);

	for(int i = 0; i<readSize; i++){
		cout<<"Char "<<i<<":"<<readBuffer[i]<<endl;
	}

	//cout<<readBuffer<<endl;

	delete readBuffer;
}

TEST(TestFileHandler, testFileSize) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	uint32 size;
	char * testFile = (char *)"/media/sdMount/adam.txt";

	size = fileHandler->fileSize(testFile);
	ASSERT_EQ(size,7);

}

TEST(TestFileHandler, testWrite) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	char * test1 = (char *) "/media/sdMount/test1.txt";
	char * test2 = (char *) "/media/sdMount/test2.txt";

	fileHandler->DeleteFile(test2);

	size_t readSize1 = 0;
	size_t readSize2 = 0;
	uint8 * readBuffer1;
	uint8 * readBuffer2;
	uint32 write1;
	uint32 write2;
	bool success = true;

	readBuffer1 = fileHandler->ReadFile(test1, &readSize1);
	EXPECT_TRUE(readBuffer1 != NULL);
	write1 = fileHandler->FileWrite(test2, (char *) readBuffer1, readSize1);
	readBuffer2 = fileHandler->ReadFile(test2, &readSize2);
	EXPECT_TRUE(readBuffer2 != NULL);

	for(uint8 i=0;i<readSize1;i++)
	{
		success &= (readBuffer1[i] == readBuffer2[i]);
	}

	ASSERT_TRUE(success);

}

TEST(DISABLED_TestFileHandler, testDelete) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	char * test1 = (char *) "/media/sdMount/test1.txt";
	fileHandler->DeleteFile(test1);

	ASSERT_TRUE(true);

}
