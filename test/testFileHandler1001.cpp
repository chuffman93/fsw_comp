#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "core/Factory.h"
#include "stdlib.h"
#include <stdio.h>

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
