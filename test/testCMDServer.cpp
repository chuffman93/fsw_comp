#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/FileSystem.h"
#include <sys/stat.h>
#include <string>
#include <stdlib.h>
#include <fstream>

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

#define TEST_DIR	"/home/root/CMDTest"
#define TAR_DIR		TEST_DIR "tar"

#define FILE1 		TEST_DIR "/test1.txt"
#define FILE2 		TEST_DIR "/test2.txt"
#define FILE3 		TEST_DIR "/test3.txt"

#define TAR_FILE1	TAR_DIR "/test1.txt"
#define TAR_FILE2	TAR_DIR "/test2.txt"
#define TAR_FILE3	TAR_DIR "/test3.txt"

#define TARBALL		"/home/root/CMDtest.tar"

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
	create_test_file_system();
	packageFiles(TARBALL, TEST_DIR, "", 5, 1);

	ASSERT_NE(access(TARBALL, F_OK), -1);

	char cmd[100];
	sprintf(cmd, "tar -xzf %s -C %s", TARBALL, TAR_DIR);
	mkdir(TAR_DIR, 0777);
	system(cmd);

	ASSERT_NE(access(TAR_DIR, F_OK), -1);
	ASSERT_NE(access(TAR_FILE1, F_OK), -1);
	ASSERT_NE(access(TAR_FILE2, F_OK), -1);
	ASSERT_NE(access(TAR_FILE3, F_OK), -1);

	delete_test_file_system();
	remove(TARBALL);
	remove(TAR_FILE1);
	remove(TAR_FILE2);
	remove(TAR_FILE3);
	remove(TAR_DIR);
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

TEST(TestCMDStdTasks, testPPE){
	// ---- Create test PPE
	string ppe_cmd1 = "echo \"SYS,mkdir /home/root/CMDTest\" >> " + string(PPE_PATH);
	string ppe_cmd2 = "echo \"SYS,echo \"test\" > /home/root/CMDTest/test1\" >> " + string(PPE_PATH);
	string ppe_cmd3 = "echo \"echo \"test\" > /home/root/CMDTest/test2\" >> " + string(PPE_PATH); // invalid
	EXPECT_NE(system(ppe_cmd1.c_str()),-1);
	EXPECT_NE(system(ppe_cmd2.c_str()),-1);
	EXPECT_NE(system(ppe_cmd3.c_str()),-1);

	// ---- Parse PPE files and execute commands
	parsePPE();
	remove(PPE_PATH);

	// ---- Verify results
	ASSERT_EQ(access("/home/root/CMDTest/test1",F_OK),0);
	ASSERT_EQ(access("/home/root/CMDTest/test2",F_OK),-1);

	// ---- Cleanup
	EXPECT_NE(system("rm -rf /home/root/CMDTest"),-1);
}

TEST(TestCMDStdTasks, TestGetNumFilesDWN){
	// ---- Create test files in a test directory
	mkdir("/home/root/CMDTest", 0777);
	mkdir("/home/root/CMDTest1", 0777);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test00"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test01"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test02"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test10"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test11"),-1);
	EXPECT_NE(system("echo \"test\" >> /home/root/CMDTest/test12"),-1);

	ASSERT_EQ(getNumFiles((char *) "/home/root/CMDTest"),6);
	ASSERT_EQ(getNumFiles((char *) "/home/root/CMDTest1"),0);

	EXPECT_NE(system("rm -rf /home/root/CMDTest"),-1);
	EXPECT_NE(system("rm -rf /home/root/CMDTest1"),-1);
}

TEST(TestCMDStdTasks, TestGetDownlinkFile){
	string f1 = "echo \"test\" >> " + string(DOWNLINK_DIRECTORY) + "/test1";
	string f2 = "echo \"test\" >> " + string(DOWNLINK_DIRECTORY) + "/test2";
	string f3 = "echo \"test\" >> " + string(DOWNLINK_DIRECTORY) + "/test3";
	string f4 = "echo \"test\" >> " + string(DOWNLINK_DIRECTORY) + "/test4";
	string f5 = "echo \"test\" >> " + string(DOWNLINK_DIRECTORY) + "/test5";
	EXPECT_NE(system(f1.c_str()),-1);
	EXPECT_NE(system(f2.c_str()),-1);
	EXPECT_NE(system(f3.c_str()),-1);
	EXPECT_NE(system(f4.c_str()),-1);
	EXPECT_NE(system(f5.c_str()),-1);

	string file1 = getDownlinkFile(1);
	string file2 = getDownlinkFile(4);
	string f1check = string(DOWNLINK_DIRECTORY) + "/test1";
	string f2check = string(DOWNLINK_DIRECTORY) + "/test4";

	ASSERT_EQ(strcmp(file1.c_str(),f1check.c_str()),0);
	ASSERT_EQ(strcmp(file2.c_str(),f2check.c_str()),0);

	string clearDWN = "rm " + string(DOWNLINK_DIRECTORY) + "/*";
	EXPECT_NE(system(clearDWN.c_str()),-1);
}
