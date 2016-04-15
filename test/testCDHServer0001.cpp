/*
 * testCDHServer0001.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "core/Factory.h"
#include "servers/DispatchStdTasks.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "stdlib.h"
#include <stdio.h>
#include <pthread.h>
#include "util/itoa.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"

using namespace std;
using namespace rel_ops;
//using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

void* taskRunCDH(void * params) {
	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(
			CDH_SERVER_SINGLETON));
	ModeManager * modeManager =
			dynamic_cast<ModeManager *>(Factory::GetInstance(
					MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*cdhServer);

	for (int i = 0; i < 0; i++) {
		//wdm->Kick();
		usleep(1000000);
	}

	printf("\r\nKicking off the CDH server\r\n");

	bool success = cdhServer->RegisterHandlers();
	cdhServer->SubsystemLoop();
	pthread_exit(NULL);
}

TEST(TestCDHServer, testSingleton) {

	Factory::GetInstance(CDH_SERVER_SINGLETON);
	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(
			CDH_SERVER_SINGLETON));

	ASSERT_TRUE(cdhServer->Exist());

}

TEST(TestCDHServer, testStub) {

	FileHandler * fileHandler =
			dynamic_cast<FileHandler *>(Factory::GetInstance(
					MODE_MANAGER_SINGLETON));
	ModeManager * modeManager =
			dynamic_cast<ModeManager *>(Factory::GetInstance(
					MODE_MANAGER_SINGLETON));
	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);

	// Create CDHServer
	pthread_t CDHThread;
	bool threadCreated = pthread_create(&CDHThread, NULL, &taskRunCDH, NULL);
	if (!threadCreated) {
		printf("CDH Server Thread Creation Success\n");
	} else {
		printf("CDH Server Thread Creation Failed\n");
	}

	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(
			CDH_SERVER_SINGLETON));
	EXPECT_TRUE(!threadCreated);

	usleep(5000000);
	//TODO: FIX THE FUCKING CODE ALEX

	/*
	VariableTypeData int_hold = VariableTypeData((uint32) 2);
	list<VariableTypeData *> params;
	params.push_back(&int_hold);
	*/

	ReturnMessage * ret1 = DispatchPacket(SERVER_LOCATION_ACS,SERVER_LOCATION_CDH, 1, 0, MESSAGE_TYPE_COMMAND, CDH_MEM_USAGE_CMD);
	/*
	 bool success = ret1->GetSuccess();
	 MessageCodeType code = ret1->GetOpcode();
	 MessageTypeEnum type = ret1->GetType();
	 cout<<"Success: "<<success<<" <------------------------------"<<endl;
	 cout<<"Opcode: "<<code<<endl;
	 cout<<"Type: "<<type<<endl;
	 */
	MessageProcess(SERVER_LOCATION_CDH, ret1);

	/*
	 usleep(1000000);
	 char * file = (char *) "/media/sdMount/CDH/CDH_182_0_0.dat";
	 uint8 * readBuffer;
	 uint32 size;
	 size = fileHandler->fileSize(file);
	 //readBuffer = fileHandler->ReadFile((char *) "/media/sdMount/CDH/CDH_182_0_0.dat", &size);

	 for(int i=0;i<(int)size;i++){
	 cout<<"Char "<<i<<": "<<readBuffer[i]<<endl;
	 }
	 */

	ASSERT_TRUE(true);
}

TEST(TestCDHServer, testSysInfo){

	struct sysinfo si;
	sysinfo (&si);

	cout<<"1 min: "<<si.loads[0]<<endl;
	cout<<"5 min: "<<si.loads[1]<<endl;
	cout<<"15 min: "<<si.loads[2]<<endl;
	cout<<"Free RAM: "<<si.freeram<<endl;
	cout<<"Total RAM: "<<si.totalram<<endl;
	cout<<"Percent used RAM: "<<(100.0*(259964928.0 - ((float) si.freeram)) / (259964928.0))<<endl;
}

TEST(TestCDHServer, testStatvfs){

	struct statvfs svfs;
	statvfs((char *) "/", &svfs);

	cout<<"Free blocks: "<<svfs.f_bfree<<endl;
	cout<<"Total blocks: "<<svfs.f_blocks<<endl;

}

TEST(TestCDHServer, testProc) {

	char * filename = (char *) "/proc/meminfo";
	FILE * fp;

	fp = fopen(filename, "r");

	EXPECT_TRUE(fp);

	uint8 * buffer = NULL;
	uint8 bufferSize = 128;
	size_t result;
	buffer = new uint8[bufferSize];
	result = fread(buffer, 1, bufferSize, fp);

	for (int i = 0; i < 128; i++) {
		cout << buffer[i];
	}

	delete buffer;
}
