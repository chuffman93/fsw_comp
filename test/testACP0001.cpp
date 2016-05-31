/*
 * testACP0001.cpp
 *
 *  Created on: Mar 9, 2016
 *      Author: alex
 */


#include <iostream>
#include "gtest/gtest.h"
#include "servers/CMDServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/Dispatcher.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

void* taskRunCMD(void * params)
{
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*cmdServer);

	for(int i = 0; i < 0; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	printf("\r\nKicking off the CMD server\r\n");

	bool success = cmdServer->RegisterHandlers();
	cmdServer->SubsystemLoop();
	pthread_exit(NULL);
}

TEST(TestACP, initialization){

	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// Check all hardware initializations
	bool success = true;
	for (int i=HARDWARE_LOCATION_MIN; i<HARDWARE_LOCATION_MAX;i++)
	{
		cout<<cmdServer->subsystem_acp_protocol[i]<<endl;
		if (cmdServer->subsystem_acp_protocol[i]==ACP_PROTOCOL_SPI)
		{
			success &= true;
		}
		else
		{
			success &= false;
		}
	}

	//Verify that all protocols are initialized to SPI
	ASSERT_EQ(success,true);
}

TEST(TestACP, protocolSwitch){

	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	modeManager->SetMode(MODE_ACCESS,LOCATION_ID_INVALID);

	// Create CMDServer
	pthread_t CMDThread;
	bool threadCreated = pthread_create(&CMDThread ,NULL,&taskRunCMD, NULL );
	if(!threadCreated)
	{
		printf("CMD Server Thread Creation Success\n");
	}
	else
	{
		printf("CMD Server Thread Creation Failed\n");
	}

	usleep(1000000);

	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	// Create Packet Params and dispatch packet
	VariableTypeData subsystem_hold((uint32) HARDWARE_LOCATION_EPS);
	VariableTypeData protocol_hold((uint32) ACP_PROTOCOL_ETH);
	list<VariableTypeData *> params;
	params.push_back(&subsystem_hold);
	params.push_back(&protocol_hold);

	ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ACS, SERVER_LOCATION_CMD, 1, 0, MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH, params);

	// Ensure that the change has been enacted
	ASSERT_EQ(cmdServer->subsystem_acp_protocol[HARDWARE_LOCATION_EPS], ACP_PROTOCOL_ETH);
	cout<<"EPS: "<<cmdServer->subsystem_acp_protocol[HARDWARE_LOCATION_EPS]<<endl;
	ASSERT_EQ(cmdServer->subsystem_acp_protocol[HARDWARE_LOCATION_ACS], ACP_PROTOCOL_SPI);
	cout<<"ACS: "<<cmdServer->subsystem_acp_protocol[HARDWARE_LOCATION_ACS]<<endl;
	EXPECT_EQ(ret->GetSuccess(), true);
	EXPECT_EQ(ret->GetOpcode(), CMD_ACP_SWITCH_SUCCESS);

	delete ret;

	/*
	// Test for bad switch

	VariableTypeData subsystem_hold2((uint32) HARDWARE_LOCATION_EPS);
	VariableTypeData protocol_hold2((uint32) ACP_PROTOCOL_MAX);
	params.clear();
	params.push_back(&subsystem_hold2);
	params.push_back(&protocol_hold2);

	ret = DispatchPacket(SERVER_LOCATION_ACS, SERVER_LOCATION_CMD, 1, 0, MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH, params);

	// Ensure that the function catches the error
	ASSERT_EQ(ret->GetSuccess(), false);
	EXPECT_EQ(ret->GetOpcode(), CMD_ACP_SWITCH_BAD_PROTOCOL);
	*/
}
