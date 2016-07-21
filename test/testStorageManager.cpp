#include "gtest/gtest.h"
#include "core/StdTypes.h"
#include "core/Factory.h"

#include "servers/CDHServer.h"
#include "servers/DispatchStdTasks.h"
#include "util/StorageManager.h"
#include "util/Logger.h"
#include "core/StdTypes.h"

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

// Necessary to test CDH
void * taskRunCDH(void * params) {

	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log("taskRunCDH", LOGGER_LEVEL_INFO);

	modeManager->Attach(*cdhServer);

	for (int i = 0; i < 0; i++) {
		//wdm->Kick();
		usleep(1000000);
	}

	logger->Log("Kicking off the CDH server", LOGGER_LEVEL_INFO);

	bool handlers = cdhServer->RegisterHandlers();
	if(!handlers)
	{
		logger->Log("Error starting CDH Handlers!", LOGGER_LEVEL_FATAL);
	}
	static_cast<SubsystemServer*>(cdhServer)->SubsystemLoop();
	pthread_exit(NULL);
}

TEST(TestStorageManager, testClean){
	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);

	// Create CDHServer ---------------------------------------------------------------------------------------------------------------
	pthread_t CDHThread;
	bool threadCreated = pthread_create(&CDHThread, NULL, &taskRunCDH, NULL);
	if (!threadCreated) {
		printf("CDH Server Thread Creation Success\n");
	} else {
		printf("CDH Server Thread Creation Failed\n");
	}

	usleep(2000000);

	uint8 message[8];
	uint32 weekStart = 20;
	uint32 weekEnd = 25;
	AddUInt32(message, weekStart);
	AddUInt32(message+4, weekEnd);

	FSWPacket * query = new FSWPacket(SERVER_LOCATION_ACS, SERVER_LOCATION_CDH, CDH_CLEAN_FS_CMD, true, false, MESSAGE_TYPE_COMMAND, 8, message);
	FSWPacket * ret = DispatchPacket(query);
	PacketProcess(SERVER_LOCATION_CDH, ret);
}
