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
#include "HAL/SPI_Server.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"

using namespace std;

int main(int argc, char * argv[]) {
	Logger * logger = (Logger *) Factory::GetInstance(LOGGER_SINGLETON);
	logger->Fatal("Entered Flight Software");

	mq_unlink("/queueHandleSPITX");
	mq_unlink("/queueHandleRX");

	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	modeManager->SetMode(MODE_BUS_PRIORITY);

	logger->Fatal("Flight software initialization complete! Starting servers!");

	// ----------------------------- Grab Server Instances ---------------------------------------------------------
	bool threadsCreated = true;
	ACSServer * acsServer = static_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	COMServer * comServer = static_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	ERRServer * errServer = static_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	PLDServer * pldServer = static_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	SCHServer * schServer = static_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	SPI_HALServer * spiServer = static_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	// ----------------------------- Start Servers -----------------------------------------------------------------
	WatchdogManager * watchdogManager = static_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	threadsCreated &= watchdogManager->StartServer(acsServer, 5,	false);	 //ACS
	threadsCreated &= watchdogManager->StartServer(cdhServer, 2,	true);	 //CDH
	threadsCreated &= watchdogManager->StartServer(cmdServer, 5,	true);	 //CMD
	threadsCreated &= watchdogManager->StartServer(comServer, 1,	false);	 //COM
	threadsCreated &= watchdogManager->StartServer(epsServer, 1,	true);	 //EPS
	threadsCreated &= watchdogManager->StartServer(errServer, 0,	true);	 //ERR
	threadsCreated &= watchdogManager->StartServer(fmgServer, 0,	true);	 //FMG
	threadsCreated &= watchdogManager->StartServer(gpsServer, 5,	false);	 //GPS
	threadsCreated &= watchdogManager->StartServer(pldServer, 5,	true);	 //PLD
	threadsCreated &= watchdogManager->StartServer(schServer, 0,	true);	 //SCH
	threadsCreated &= watchdogManager->StartServer(spiServer, 0,	true);	 //SPI

	if (!threadsCreated) {
		logger->Fatal("Not all threads were created on startup!");
		TLM_STARTUP_ERROR();
		sleep(5); // attempt to let the FMGServer log the error before rebooting
		system("reboot");
	} else {
		logger->Info("All servers created!");
		TLM_SERVERS_CREATED();
	}

	watchdogManager->WatchdogManagerTask();

	logger->Fatal("Flight Software exiting from main! Watchdog exited!");
	TLM_MAIN_EXIT();
	sleep(5); // attempt to let the FMGServer log the error before rebooting
	system("reboot");
}
