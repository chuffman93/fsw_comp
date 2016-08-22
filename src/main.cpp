#include "POSIX.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/COMServer.h"
#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/CDHServer.h"
#include "servers/SubsystemServer.h"
#include "servers/ERRServer.h"
#include "servers/ErrorQueue.h"
#include "servers/DispatchStdTasks.h"
#include "HAL/Ethernet_Server.h"
#include "HAL/SPI_Server.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

int main(int argc, char * argv[])
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("Entered Flight Software", LOGGER_LEVEL_FATAL);

	mq_unlink("/queueHandleSPITX");
	mq_unlink("/queueHandleRX");

	Factory::GetInstance(DISPATCHER_SINGLETON);
	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	Factory::GetInstance(ERROR_QUEUE_SINGLETON);
	Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON);
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	modeManager->SetMode(MODE_STARTUP, LOCATION_ID_INVALID);

	logger->Log("Flight software initialization complete! Starting servers!", LOGGER_LEVEL_FATAL);

	// ----------------------------- Grab Server Instances ---------------------------------------------------------
	bool threadsCreated = true;
	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	ERRServer * errServer = dynamic_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	SPI_HALServer * spiServer = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	// ----------------------------- Start Servers -----------------------------------------------------------------
	threadsCreated &= WatchdogManager::StartServer(acsServer, 100,	false);	 //ACS
	threadsCreated &= WatchdogManager::StartServer(cdhServer, 20,	true);	 //CDH
	threadsCreated &= WatchdogManager::StartServer(cmdServer, 50,	false);	 //CMD
	threadsCreated &= WatchdogManager::StartServer(comServer, 10,	true);	 //COM
	threadsCreated &= WatchdogManager::StartServer(epsServer, 0,	false);	 //EPS
	threadsCreated &= WatchdogManager::StartServer(errServer, 0,	false);	 //ERR
	threadsCreated &= WatchdogManager::StartServer(gpsServer, 50,	false);	 //GPS
	threadsCreated &= WatchdogManager::StartServer(pldServer, 50,	false);	 //PLD
	threadsCreated &= WatchdogManager::StartServer(schServer, 0,	false);	 //SCH
	threadsCreated &= WatchdogManager::StartServer(spiServer, 0,	true);	 //SPI

	if(!threadsCreated){
		logger->Log("Not all threads were created on startup!", LOGGER_LEVEL_FATAL);
		// TODO: do something?
	}else{
		logger->Log("All servers created!", LOGGER_LEVEL_INFO);
	}

	//WatchdogManager::WatchdogManagerTask();
	while(true){usleep(100000);}

	logger->Log("Flight Software exiting from main! All relevant threads have exited!", LOGGER_LEVEL_FATAL);

	return 42;
}
