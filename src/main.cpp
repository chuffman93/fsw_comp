#include "POSIX.h"
#include "servers/ACSServer.h"
#include "servers/CDHServer.h"
#include "servers/CMDServer.h"
#include "servers/COMServer.h"
#include "servers/FMGServer.h"
#include "servers/GPSServer.h"
#include "servers/EPSServer.h"
#include "servers/ERRServer.h"
#include "servers/PLDServer.h"
#include "servers/SCHServer.h"
#include "servers/SubsystemServer.h"
#include "servers/DispatchStdTasks.h"
#include "HAL/SPI_Server.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

int main(int argc, char * argv[]) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "Entered Flight Software");

	mq_unlink("/queueHandleSPITX");
	mq_unlink("/queueHandleRX");

	Factory::GetInstance(DISPATCHER_SINGLETON);
	Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON);
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	modeManager->SetMode(MODE_BUS_PRIORITY);

	logger->Log(LOGGER_LEVEL_FATAL, "Flight software initialization complete! Starting servers!");

	// ----------------------------- Grab Server Instances ---------------------------------------------------------
	bool threadsCreated = true;
	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	ERRServer * errServer = dynamic_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	SPI_HALServer * spiServer = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	// ----------------------------- Start Servers -----------------------------------------------------------------
	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	threadsCreated &= watchdogManager->StartServer(acsServer, 5,	false);	 //ACS
	threadsCreated &= watchdogManager->StartServer(cdhServer, 2,	true);	 //CDH
	threadsCreated &= watchdogManager->StartServer(cmdServer, 5,	true);	 //CMD
	threadsCreated &= watchdogManager->StartServer(comServer, 1,	false);	 //COM
	threadsCreated &= watchdogManager->StartServer(epsServer, 1,	true);	 //EPS
	threadsCreated &= watchdogManager->StartServer(errServer, 0,	true);	 //ERR
	threadsCreated &= watchdogManager->StartServer(fmgServer, 0,	true);	 //FMG
	threadsCreated &= watchdogManager->StartServer(gpsServer, 5,	false);	 //GPS
	threadsCreated &= watchdogManager->StartServer(pldServer, 5,	false);	 //PLD
	threadsCreated &= watchdogManager->StartServer(schServer, 0,	true);	 //SCH
	threadsCreated &= watchdogManager->StartServer(spiServer, 0,	true);	 //SPI

	if (!threadsCreated) {
		logger->Log(LOGGER_LEVEL_FATAL, "Not all threads were created on startup!");
		TLM_STARTUP_ERROR();
		// TODO: do something?
	} else {
		logger->Log(LOGGER_LEVEL_INFO, "All servers created!");
		TLM_SERVERS_CREATED();
	}

	watchdogManager->WatchdogManagerTask();

	logger->Log(LOGGER_LEVEL_FATAL, "Flight Software exiting from main! Watchdog exited!");

	return 42;
}
