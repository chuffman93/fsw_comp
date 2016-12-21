#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include <sys/stat.h>
#include <string>

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

#define TEST_DIR "/home/root/jacktest/CMDTest"
#define FILE1 TEST_DIR "/test1.txt"
#define FILE2 TEST_DIR "/test2.txt"
#define FILE3 TEST_DIR "/test3.txt"

void create_test_file_system(){
	mkdir(TEST_DIR, 0777);

	ofstream outputFile1(FILE1);
	ofstream outputFile2(FILE2);
	ofstream outputFile3(FILE3);

	outputFile1 << "12";
	outputFile2 << "assdf";

	outputFile1.close();
	outputFile2.close();
	outputFile3.close();
}

void delete_test_file_system(){
	remove(FILE1);
	remove(FILE2);
	remove(FILE3);
	remove(TEST_DIR);
}

// ------- TESTS --------------------------------------------------------------
TEST(TestCMDStdTasks, testTrimNewline){
	const char* test_str1 = "hello\n";
	const char* test_str2 = "hello";

	const char* trimmed_str1 = trimNewline(test_str1).c_str();
	const char* trimmed_str2 = trimNewline(test_str2).c_str();

	ASSERT_NE(strcmp(trimmed_str1, "hello\n"), 0);
	ASSERT_EQ(strcmp(trimmed_str1, "hello\0"), 0);
	ASSERT_EQ(strcmp(trimmed_str2, "hello"), 0);

}

TEST(TestCMDStdTasks, testGetFileSize){
	int size;

	create_test_file_system();

	size = getFileSize(TEST_DIR,"",5);
	ASSERT_EQ(size, 7);

	size = getFileSize(TEST_DIR,"test1.txt",5);
	ASSERT_EQ(size, 2);

	size = getFileSize(TEST_DIR,"",0);
	ASSERT_EQ(size, 0);

	delete_test_file_system();
}

TEST(TestCMDStdTasks, testPackageFiles){
	// NOTE: This test only checks the the tar exists,
	// Need to manually untar the file and check that it is correct

	create_test_file_system();

	const char * tar_file = "/home/root/jacktest/CMDtest.tar";
	packageFiles(tar_file, TEST_DIR, "", 5);

	const int file_exists = access(tar_file, F_OK);
	ASSERT_NE(file_exists, -1);

	delete_test_file_system();
	remove(tar_file);
}
