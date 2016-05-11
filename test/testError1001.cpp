/*
 * testError1001.cpp
 *
 *  Created on: May 10, 2016
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

#include "servers/ErrorOctopus.h"
#include "servers/ErrorStdTasks.h"
#include "servers/ErrorHandlers.h"

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

// Necessary to run the error server
void * taskRunERR(void * params)
{
	ErrorOctopus * errServer = dynamic_cast<ErrorOctopus *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	modeManager->Attach(*errServer);

	errServer->RegisterHandlers();
	errServer->SubsystemLoop();
	pthread_exit(NULL);
}

// Successful
TEST(TestErrorHandling, testLog)
{
	// Grab necessary instances, set mode ---------------------------------------------------------------------------------------
	FileHandler * fileHandler = dynamic_cast<FileHandler *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	modeManager->SetMode(MODE_ERROR, LOCATION_ID_INVALID);


	// Release the error octopus ------------------------------------------------------------------------------------------------
	pthread_t ERRThread;
	bool threadCreated = pthread_create(&ERRThread, NULL, &taskRunERR, NULL);
	if (!threadCreated) {
		printf("CDH Server Thread Creation Success\n");
	} else {
		printf("CDH Server Thread Creation Failed\n");
	}

	ErrorOctopus * errServer = dynamic_cast<ErrorOctopus *>(Factory::GetInstance(ERR_SERVER_SINGLETON));
	EXPECT_TRUE(!threadCreated);
	usleep(5000000);

	// Send and process error message for each server ----------------------------------------------------------------------------
	int errors[] = {CDH_CPU_USAGE_FAILURE, CMD_ACP_SWITCH_FAILURE, EPS_HS_FAILURE, ACS_HS_FAILURE, GPS_HS_FAILURE, COM_HS_FAILURE, PLD_HS_FAILURE, SCH_BUILD_SCHEDULE_FAILURE, THM_HS_FAILURE};
	int servers[] = {SERVER_LOCATION_CDH, SERVER_LOCATION_CMD, SERVER_LOCATION_EPS, SERVER_LOCATION_ACS, SERVER_LOCATION_GPS, SERVER_LOCATION_COM, SERVER_LOCATION_PLD, SERVER_LOCATION_SCH, SERVER_LOCATION_THM};
	for(int i=0; i<9; i++)
	{
		ErrorMessage err(errors[i]);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		MessageProcess(servers[i], ret);
		usleep(1000000);
	}

	// Note: To verify test success, must check filehandler output!

}
