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

TEST(TestFileHandler, exist) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	ASSERT_FALSE(fileHandler->FileExists("noooope.txt"));
	ASSERT_TRUE(fileHandler->FileExists("CMD_List_1.dat")); //this file should be in home directory
	EXPECT_EQ(fileHandler->fileSize("CMD_List_1.dat"),0);
}

TEST(TestFileHandler, sdNavRead) {

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));

	ASSERT_TRUE(fileHandler->FileExists("/media/sdMount/test.c"));

	uint32 size = fileHandler->fileSize("/media/sdMount/test.c");
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
