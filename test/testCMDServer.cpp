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

TEST(TestCMDStdTasks, testTrimNewline){
	char* test_str = "hello\n";
	//printf("before: %s\n", test_str);
	//printf("trimming: %s\n", trimNewline(test_str).c_str());
	ASSERT_NE(trimNewline(test_str), "hello\n");
}

TEST(TestCMDStdTasks, testGetFileSize){
	mkdir("/home/root/CMDTest", 0777);
	ofstream outputFile1("/home/root/CMDTest/test1.txt");
	ofstream outputFile2("/home/root/CMDTest/test2.txt");
	ofstream outputFile3("/home/root/CMDTest/test3.txt");
	outputFile1 << "12";
	outputFile2 << "assdf";
	outputFile1.close();
	outputFile2.close();
	outputFile3.close();

	int size;
	size = getFileSize("/home/root/CMDTest","",5);
	ASSERT_EQ(size, 7);

	size = getFileSize("/home/root/CMDTest","test1.txt",5);
	ASSERT_EQ(size, 2);

	size = getFileSize("/home/root/CMDTest","",0);
	ASSERT_EQ(size, 0);

	remove("/home/root/CMDTest/test1.txt");
	remove("/home/root/CMDTest/test2.txt");
	remove("/home/root/CMDTest/test3.txt");
	remove("/home/root/CMDTest");
}

TEST(TestCMDStdTasks, testPackageFiles){
	mkdir("/home/root/CMDTest", 0777);
	ofstream outputFile1("/home/root/CMDTest/test1.txt");
	ofstream outputFile2("/home/root/CMDTest/test2.txt");
	ofstream outputFile3("/home/root/CMDTest/test3.txt");
	outputFile1 << "12";
	outputFile2 << "assdf";
	outputFile1.close();
	outputFile2.close();
	outputFile3.close();

	packageFiles("/home/root/CMDtest.tar", "/home/root/CMDTest", "", 5);

	// Tarball must be verified manually - this test will always pass

	remove("/home/root/CMDTest/test1.txt");
	remove("/home/root/CMDTest/test2.txt");
	remove("/home/root/CMDTest/test3.txt");
	remove("/home/root/CMDTest");
}
