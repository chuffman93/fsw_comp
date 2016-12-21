#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "servers/DispatchStdTasks.h"
#include "servers/ACSStdTasks.h"
#include "core/Factory.h"
#include "stdlib.h"
#include <stdio.h>
//#include "core/DataMessage.h"
//#include "core/ReturnMessage.h"
//#include "core/ErrorMessage.h"

using namespace std;
using namespace rel_ops;
//using namespace AllStar::HAL;
using namespace AllStar::Core;


/*TEST(TestFileHandler, testAppend) {

//	Factory::GetInstance(FILE_HANDLER_SINGLETON);
//	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
//
//	bool success = true;
//
//	// Try ACS error message
//	ErrorMessage * msg = new ErrorMessage(ACS_HS_FAILURE);
//	ReturnMessage * ret = new ReturnMessage(msg, false);
//	MultiDataMessage * dataMessage = dynamic_cast<MultiDataMessage *> (ret->GetMessagePtr());
//	success &= fileHandler->Log(SUBSYSTEM_ACS,ACS_HS_FAILURE, (* dataMessage));
//
//	// Try CMD error message
//	for(int i=0;i<105;i++){
//		ErrorMessage * msg2 = new ErrorMessage(CMD_ACP_SWITCH_FAILURE);
//		ReturnMessage * ret2 = new ReturnMessage(msg2, false);
//		MultiDataMessage * dataMessage2 = dynamic_cast<MultiDataMessage *> (ret2->GetMessagePtr());
//		success &= fileHandler->Log(SUBSYSTEM_CMD,CMD_ACP_SWITCH_FAILURE, (* dataMessage2));
//		fileHandler->week++;
//	}
//
//
//	ASSERT_TRUE(success);

}

TEST(TestFileHandler, testRead) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	size_t readSize = 0;
	uint8 * readBuffer;
	char * testFile = (char *) "/media/sdMount/adam.txt";

	// readBuffer returns num char read
	readBuffer = fileHandler->ReadFile(testFile, &readSize);
	ASSERT_TRUE(readBuffer != NULL);

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

	// This way we ensure that the write function creates a file if it doesn't exist
	fileHandler->DeleteFile(test2);

	size_t readSize1 = 0;
	size_t readSize2 = 0;
	uint8 * readBuffer1;
	uint8 * readBuffer2;
	char writeBuffer1[] = {'T','e','s','t','2'};
	uint32 write1;
	uint32 write2;
	bool success = true;

	// Read from test1.txt, and write that to test2.txt (DNE)
	readBuffer1 = fileHandler->ReadFile(test1, &readSize1);
	write1 = fileHandler->FileWrite(test2, (char *) readBuffer1, readSize1);
	readBuffer2 = fileHandler->ReadFile(test2, &readSize2);
	write2 = fileHandler->FileWrite(test2, (char *) writeBuffer1, 5);

	// Determine Test Success
	EXPECT_TRUE(readBuffer1 != NULL);
	EXPECT_TRUE(readBuffer2 != NULL);
	success &= (write1 == readSize1);
	success &= (write2 == write1+5);
	for(uint8 i=0;i<readSize1;i++)
	{
		success &= (readBuffer1[i] == readBuffer2[i]);
	}

	// Cleanup and Result
	delete readBuffer1;
	delete readBuffer2;
	ASSERT_TRUE(success);

}

TEST(TestFileHandler, testDelete) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	bool success = true;
	int sys = system("touch /media/sdMount/test_delete.txt");
	char * test1 = (char *) "/media/sdMount/test_delete.txt";
	char * fake = (char *) "/media/sdMount/fake.txt";
	success &= fileHandler->DeleteFile(test1);
	success &= !fileHandler->DeleteFile(fake);
	success &= (sys != -1);

	ASSERT_TRUE(success);

}*/
