/*
 * testHandlers.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: alex
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

#include "servers/ACSServer.h"
#include "servers/ACSStdTasks.h"

using namespace std;
using namespace rel_ops;
//using namespace AllStar::HAL;
using namespace AllStar::Core;
using namespace AllStar::Servers;

//void* taskRunACS(void * params) {
//	ACSServer * acsServer = static_cast<ACSServer *>(Factory::GetInstance(
//			ACS_SERVER_SINGLETON));
//	ModeManager * modeManager =
//			static_cast<ModeManager *>(Factory::GetInstance(
//					MODE_MANAGER_SINGLETON));
//	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
//
//	modeManager->Attach(*acsServer);
//
//	for (int i = 0; i < 0; i++) {
//		//wdm->Kick();
//		usleep(1000000);
//	}
//
//	printf("\r\nKicking off the ACS server\r\n");
//
//	bool success = acsServer->RegisterHandlers();
//	acsServer->SubsystemLoop();
//	pthread_exit(NULL);
//}

TEST(TestHandlers, test1){
//	FileHandler * fileHandler =
//			dynamic_cast<FileHandler *>(Factory::GetInstance(
//					MODE_MANAGER_SINGLETON));
//	ModeManager * modeManager =
//			static_cast<ModeManager *>(Factory::GetInstance(
//					MODE_MANAGER_SINGLETON));
//	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);
//
//	// Create ACSServer
//	pthread_t ACSThread;
//	bool threadCreated = pthread_create(&ACSThread, NULL, &taskRunACS, NULL);
//	if (!threadCreated) {
//		printf("ACS Server Thread Creation Success\n");
//	} else {
//		printf("ACS Server Thread Creation Failed\n");
//	}
//
//	ACSServer * acsServer = static_cast<ACSServer *>(Factory::GetInstance(
//			ACS_SERVER_SINGLETON));
//	EXPECT_TRUE(!threadCreated);
//
//	usleep(5000000);
//
//	ReturnMessage * ret1 = DispatchPacket(SERVER_LOCATION_CDH,SERVER_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_HS_CMD);
//	/*
//	 bool success = ret1->GetSuccess();
//	 MessageCodeType code = ret1->GetOpcode();
//	 MessageTypeEnum type = ret1->GetType();
//	 cout<<"Success: "<<success<<" <------------------------------"<<endl;
//	 cout<<"Opcode: "<<code<<endl;
//	 cout<<"Type: "<<type<<endl;
//	 */
//	MessageProcess(SERVER_LOCATION_ACS, ret1);
}


