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

// link to DLT format: http://thinker.colorado.edu/projects/fsw/wiki/Deletion_File
TEST(TestCMDStdTasks, testDLT){
	// ---- Create test files in a test directory
	mkdir("/home/root/CMDTest", 0777);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test00"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test01"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test02"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test10"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test11"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test12"),-1);

	// ---- Create test DLT
	string dlt_cmd1 = "echo \"/home/root/CMDTest,-1,test1.\" >> " + string(DLT_PATH); 	// this line should delete all files matching "test1."
	string dlt_cmd2 = "echo \"/home/root/CMDTest,2,X\" >> " + string(DLT_PATH); 		// this line should delete the oldest 2 files
	string dlt_cmd3 = "echo \"/home/root/CMDTest,6,test0.\" >> " + string(DLT_PATH); 	// this line is invalid, and should be ignored
	string dlt_cmd4 = "echo \"/home/root/CMDTest,-1,X\" >> " + string(DLT_PATH); 		// this line is invalid, and should be ignored
	EXPECT_NE(system(dlt_cmd1.c_str()),-1);
	EXPECT_NE(system(dlt_cmd2.c_str()),-1);
	EXPECT_NE(system(dlt_cmd3.c_str()),-1);
	EXPECT_NE(system(dlt_cmd4.c_str()),-1);

	// ---- Parse the DLT file and delete files accordingly
	parseDLT();
	remove(DLT_PATH);

	// ---- Check that files have been successfully deleted
	ASSERT_EQ(access("/home/root/CMDTest/test00", F_OK),-1); // deleted by line 2
	ASSERT_EQ(access("/home/root/CMDTest/test01", F_OK),-1); // deleted by line 2
	ASSERT_EQ(access("/home/root/CMDTest/test02", F_OK),0);  // should not have been deleted
	ASSERT_EQ(access("/home/root/CMDTest/test10", F_OK),-1); // deleted by line 1
	ASSERT_EQ(access("/home/root/CMDTest/test11", F_OK),-1); // deleted by line 1
	ASSERT_EQ(access("/home/root/CMDTest/test12", F_OK),-1); // deleted by line 1

	// ---- Cleanup
	remove("/home/root/CMDTest/test00");
	remove("/home/root/CMDTest/test01");
	remove("/home/root/CMDTest/test02");
	remove("/home/root/CMDTest/test10");
	remove("/home/root/CMDTest/test11");
	remove("/home/root/CMDTest/test12");
	remove("/home/root/CMDTest");
}

// link to DRF format: http://thinker.colorado.edu/projects/fsw/wiki/Downlink_Request
TEST(TestCMDStdTasks, testDRF){
	// ---- Create test files in a test directory
	mkdir("/home/root/CMDTest", 0777);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test00"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test01"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test02"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test10"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test11"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test12"),-1);

	// ---- Create test DLT
	string dlt_cmd1 = "echo \"arch1,/home/root/CMDTest,2,test0.\" >> " + string(DRF_PATH); 	// this line should grab the newest 2 files matching "test0."
	string dlt_cmd2 = "echo \"arch2,/home/root/CMDTest,2,X\" >> " + string(DRF_PATH); 		// this line should grab the newest 2 files
	string dlt_cmd3 = "echo \"arch3,/home/root/CMDTest,asdf,test0.\" >> " + string(DRF_PATH); 	// this line is invalid, and should be ignored
	EXPECT_NE(system(dlt_cmd1.c_str()),-1);
	EXPECT_NE(system(dlt_cmd2.c_str()),-1);
	EXPECT_NE(system(dlt_cmd3.c_str()),-1);

	// ---- Parse the DLT file and delete files accordingly
	parseDRF();
	remove(DRF_PATH);

	// ---- Tarball contents must be verified manually.
	string arch1dir = string(DOWNLINK_DIRECTORY) + "/arch1.000";
	string arch2dir = string(DOWNLINK_DIRECTORY) + "/arch2.000";
	string arch3dir = string(DOWNLINK_DIRECTORY) + "/arch3.000";
	ASSERT_EQ(access(arch1dir.c_str(), F_OK),0); // should have been created
	ASSERT_EQ(access(arch2dir.c_str(), F_OK),0); // should have been created
	ASSERT_EQ(access(arch3dir.c_str(), F_OK),-1); // should not have been created

	// ---- Cleanup
	remove("/home/root/CMDTest/test00");
	remove("/home/root/CMDTest/test01");
	remove("/home/root/CMDTest/test02");
	remove("/home/root/CMDTest/test10");
	remove("/home/root/CMDTest/test11");
	remove("/home/root/CMDTest/test12");
	remove("/home/root/CMDTest");
	string rma1 = "rm " + string(DOWNLINK_DIRECTORY) + "/a*";
	EXPECT_NE(system(rma1.c_str()),-1);
}
