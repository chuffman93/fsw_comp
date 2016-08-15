#include "POSIX.h"
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/COMServer.h"
#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/CDHServer.h"
#include "servers/ErrorOctopus.h"
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

#define DEBUG

//---------------------------- Set which servers run --------------------------------------------------------------

#define ACS_EN 0
#define CDH_EN 0
#define CMD_EN 1
#define COM_EN 0
#define EPS_EN 0
#define ERR_EN 0
#define GPS_EN 0
#define PLD_EN 0
#define SCH_EN 0

#define ETH_EN 0
#define SPI_EN 0

//---------------------------- Create server tasks ----------------------------------------------------------------
//TODO:Add meaningful exit information to each server pthread_exit

void * taskRunEPS(void * params)
{
	//EnablePinInterrupt(INTERRUPT_PIN_EPS, INTERRUPT_EDGE_RISING);

	EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*epsServer);

	logger->Log("Kicking off the EPS server", LOGGER_LEVEL_INFO);

	bool handlers = epsServer->RegisterHandlers();
	if(!handlers){
		logger->Log("EPS Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	static_cast<SubsystemServer*>(epsServer)->SubsystemLoop();
	pthread_exit(NULL);

}

void * taskRunACS(void * params)
{
	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*acsServer);

	for(int i = 0; i < 5; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	logger->Log("Kicking off the ACS server", LOGGER_LEVEL_INFO);

	bool handlers = acsServer->RegisterHandlers();
	if(!handlers){
		logger->Log("ACS Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	static_cast<SubsystemServer*>(acsServer)->SubsystemLoop();
	pthread_exit(NULL);

}

void * taskRunPLD(void * params)
{
	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*pldServer);

	for(int i = 0; i < 10; i++)
	{
		//wdm->Kick();
		usleep(100000);
	}

	logger->Log("Kicking off the PLD server", LOGGER_LEVEL_INFO);

	bool handlers = pldServer->RegisterHandlers();
	if(!handlers){
		logger->Log("PLD Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}

	static_cast<SubsystemServer*>(pldServer)->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunCOM(void * params)
{
	COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*comServer);

	for(int i = 0; i < 25; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	logger->Log("Kicking off the COM server", LOGGER_LEVEL_INFO);

	bool handlers = comServer->RegisterHandlers();
	if(!handlers){
		logger->Log("COM Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}

	static_cast<SubsystemServer*>(comServer)->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunGPS(void * params)
{
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*gpsServer);

	for(int i = 0; i < 3; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	logger->Log("Kicking off the GPS server", LOGGER_LEVEL_INFO);

	bool handlers = gpsServer->RegisterHandlers();
	if(!handlers){
		logger->Log("GPS Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}

	gpsServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunERR(void * params)
{
	ErrorOctopus * errServer = dynamic_cast<ErrorOctopus *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*errServer);
	//wdm->Kick();

	logger->Log("Kicking off the ERR server", LOGGER_LEVEL_INFO);

	bool handlers = errServer->RegisterHandlers();
	if(!handlers){
		logger->Log("ERR Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	errServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunSCH(void * params)
{
	SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*schServer);

	for(int i = 0; i < 1; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	logger->Log("Kicking off the SCH server", LOGGER_LEVEL_INFO);

	bool handlers = schServer->RegisterHandlers();
	if(!handlers){
		logger->Log("SCH Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	schServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunCMD(void * params)
{
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*cmdServer);

	for(int i = 0; i < 1; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	logger->Log("Kicking off the CMD server", LOGGER_LEVEL_INFO);

	cmdServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunCDH(void * params) {

	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

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
	//cdhServer->PrepHSPM();
	static_cast<SubsystemServer*>(cdhServer)->SubsystemLoop();
	pthread_exit(NULL);
}

void * StartETH_HAL(void * params)
{
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	ETH_HALServer * cmd_Server = dynamic_cast<ETH_HALServer *> (Factory::GetInstance(ETH_HALSERVER_SINGLETON));

	for(int i = 0; i < 10; i++)
	{
		//wdm->Kick();
		usleep(100000);
	}

	logger->Log("Kicking off the ETH_HAL server", LOGGER_LEVEL_INFO);

	cmd_Server->ETH_HALServerLoop();
	pthread_exit(NULL);
}

void * StartSPI_HAL(void * params)
{
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	SPI_HALServer * spi_server = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

//	for(int i = 0; i < 10; i++)
//	{
//		//wdm->Kick();
//		usleep(100000);
//	}

	logger->Log("Kicking off the SPI HAL server", LOGGER_LEVEL_INFO);
	spi_server->SPI_HALServerLoop();
	pthread_exit(NULL);
}

//----------------------------------- Main ------------------------------------------------------------------------

int main(int argc, char * argv[])
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("Entered Flight Software", LOGGER_LEVEL_FATAL);
/*
	PowerInit();
	RTCSetTime(0);
*/

	system("~/kernelmod.sh");

	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	mq_unlink("/errQueueHandle");
	//watchdog manager must be called first
	//Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON);
	Dispatcher * disp = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	Factory::GetInstance(ERROR_QUEUE_SINGLETON);
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

//	uint8 input = 1;
//	//GetPin(EGSE_PRESENT, PIN_TYPE_GPIO, &input);
//	if(input == 1)
//	{
//		//EGSE is plugged in, go into access mode
//		modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);
//		logger->Log("Access Mode Entered!", LOGGER_LEVEL_INFO);
//	}
//	else
//	{
//		//Flight go to startup mode
//		modeManager->SetMode(MODE_STARTUP, LOCATION_ID_INVALID);
//		logger->Log("Startup Mode Entered!", LOGGER_LEVEL_INFO);
//	}

	modeManager->SetMode(MODE_STARTUP, LOCATION_ID_INVALID);

	logger->Log("Flight software initialization complete! Starting servers!", LOGGER_LEVEL_FATAL);

	bool threadCreated;

#if ACS_EN
	// ------------------------------------- ACS Thread -------------------------------------------------------------------------
	pthread_t ACSThread;
	threadCreated = pthread_create(&ACSThread, NULL, &taskRunACS, NULL);
	if(!threadCreated)
	{
		logger->Log("ACS Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("ACS Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //ACS_EN

#if EPS_EN
	// ------------------------------------- EPS Thread -------------------------------------------------------------------------
	pthread_t EPSThread;
	threadCreated = pthread_create(&EPSThread, NULL, &taskRunEPS, NULL);

 	if(!threadCreated)
	{
 		logger->Log("EPS Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("EPS Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //EPS_EN

#if PLD_EN
	// ------------------------------------- PLD Thread -------------------------------------------------------------------------
	pthread_t PLDThread;
	threadCreated = pthread_create(&PLDThread, NULL, &taskRunPLD, NULL);
	if(!threadCreated)
	{
		logger->Log("PLD Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("PLD Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //PLD_EN

#if ERR_EN
	// ------------------------------------- ERR Thread -------------------------------------------------------------------------
	//CREATE_TASK(taskRunERR, (const signed char* const)"ERR task", 2000, NULL, 0, NULL);
	pthread_t ERRThread;
	threadCreated = pthread_create(&ERRThread, NULL, &taskRunERR, NULL);
	if(!threadCreated)
	{
		logger->Log("ERR Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("ERR Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //ERR_EN

#if GPS_EN
	// ------------------------------------- GPS Thread -------------------------------------------------------------------------
	pthread_t GPSThread;
	threadCreated = pthread_create(&GPSThread, NULL, &taskRunGPS, NULL);
	if(!threadCreated)
	{
		logger->Log("GPS Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("GPS Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //GPS_EN

#if SCH_EN
	// -------------------------------------SCH Thread-------------------------------------------------------------------------
	pthread_t SCHThread;
	threadCreated = pthread_create(&SCHThread, NULL, &taskRunSCH, NULL);
	if(!threadCreated)
	{
		logger->Log("SCH Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("SCH Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif

#if CMD_EN
	// ------------------------------------- CMD Thread -------------------------------------------------------------------------
	pthread_t CMDThread;
	threadCreated = pthread_create(&CMDThread ,NULL,&taskRunCMD, NULL );
	if(!threadCreated)
	{
		logger->Log("CMD Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("CMD Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //CMD_EN

#if CDH_EN
	// ------------------------------------- CDH Thread -------------------------------------------------------------------------
	pthread_t CDHThread;
	threadCreated = pthread_create(&CDHThread ,NULL,&taskRunCDH, NULL );
	if(!threadCreated)
	{
		logger->Log("CDH Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("CDH Server Thread Creation Failed\n", LOGGER_LEVEL_FATAL);
	}
#endif //CDH_EN

#if ETH_EN
	// ------------------------------------- ETH Thread -------------------------------------------------------------------------
	pthread_t ETHThread;
	threadCreated = pthread_create(&ETHThread ,NULL,&StartETH_HAL, NULL );
	if(!threadCreated)
	{
		logger->Log("Ethernet HALServer Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("Ethernet HALServer Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //ETH_EN

#if SPI_EN
	// ------------------------------------- SPI Thread -------------------------------------------------------------------------
	pthread_t SPIThread;
	threadCreated = pthread_create(&SPIThread ,NULL,&StartSPI_HAL, NULL );

	if(!threadCreated)
	{
		logger->Log("SPI HALServer Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("SPI HALServer Thread Creation Failed", LOGGER_LEVEL_FATAL);
	}
#endif //SPI_EN

	logger->Log("All servers created!", LOGGER_LEVEL_INFO);

	// Suspend execution of main until the following threads exit
#if ACS_EN
	pthread_join(ACSThread, NULL);
#endif //ACS_EN

#if EPS_EN
	pthread_join(EPSThread, NULL);
#endif //EPS_EN

#if ERR_EN
	pthread_join(ERRThread, NULL);
#endif //ERR_EN

#if GPS_EN
	pthread_join(GPSThread, NULL);
#endif

#if SCH_EN
	pthread_join(SCHThread, NULL);
#endif

#if PLD_EN
	pthread_join(PLDThread, NULL);
#endif //PLD_EN

#if CDH_EN
	pthread_join(CDHThread, NULL);
#endif //CDH_EN

#if CMD_EN
	pthread_join(CMDThread, NULL);
#endif
	logger->Log("Flight Software exiting from main! All relevant threads have exited!", LOGGER_LEVEL_FATAL);

	return 42;
}
