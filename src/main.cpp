/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.6.1 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
*
* \brief USART example application for AVR32 USART driver.
*
* - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
* - Supported devices:  All AVR32 devices with a USART module can be used.
* - AppNote:
*
* \author               Atmel Corporation: http://www.atmel.com \n
*                       Support and FAQ: http://support.atmel.no/
*
******************************************************************************/

/*! \page License
* Copyright (c) 2009 Atmel Corporation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an Atmel
* AVR product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
*/

/*! \mainpage
* \section intro Introduction
* This is the documentation for the data structures, functions, variables,
* defines, enums, and typedefs for the USART software driver.\n It also comes
* bundled with an example. This example is a basic Hello-World example.\n
* <b>Example's operating mode: </b>
* -# A message is displayed on the PC terminal ("Hello, this is AT32UC3 saying hello! (press enter)")
* -# You may then type any character other than CR(Carriage Return) and it will
* be echoed back to the PC terminal.
* -# If you type a CR, "'CRLF'Goodbye." is echoed back to the PC terminal and
* the application ends.
*
* \section files Main Files
* - usart.c: USART driver;
* - usart.h: USART driver header file;
* - usart_example.c: USART example application.
*
* \section compilinfo Compilation Information
* This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
* for Atmel AVR32. Other compilers may or may not work.
* 
* \section deviceinfo Device Information
* All AVR32 devices with a USART module can be used.
*
* \section configinfo Configuration Information
* This example has been tested with the following configuration:
* - EVK1100, EVK1101, UC3C_EK, EVK1104, EVK1105 evaluation kits; STK600+RCUC3L routing card; AT32UC3L-EK
* - CPU clock:
*        -- 12 MHz : EVK1100, EVK1101, EVK1104, EVK1105, AT32UC3L-EK evaluation kits; STK600+RCUC3L routing card
*        -- 16 Mhz : AT32UC3C-EK
* - USART1 (on EVK1100 or EVK1101) connected to a PC serial port via a standard
*   RS232 DB9 cable, or USART0 (on EVK1105) or USART2 (on AT32UC3C-EK) or USART1 (on EVK1104)
*   or USART3 (on AT32UC3L-EK) abstracted with a USB CDC connection to a PC;
*   STK600 usart port for the STK600+RCUC3L setup (connect STK600.PE2 to STK600.RS232 SPARE.TXD
*   and STK600.PE3 to STK600.RS232 SPARE.RXD)
* - PC terminal settings:
*   - 57600 bps,
*   - 8 data bits,
*   - no parity bit,
*   - 1 stop bit,
*   - no flow control.
*
* \section contactinfo Contact Information
* For further information, visit
* <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
* Support and FAQ: http://support.atmel.no/
*/
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
#include "servers/THMServer.h"
#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/CDHServer.h"
#include "servers/ErrorOctopus.h"
#include "servers/ErrorQueue.h"
#include "HAL/Ethernet_Server.h"
#include "HAL/SPI_Server.h"

//extern "C"
//{
//	#include <avr32/io.h>
//	#include "utils/compiler.h"
//	#include "drivers/power_clocks_lib.h"
//	#include "drivers/gpio.h"
//	#include "drivers/usart.h"
//	#include "drivers/mpu.h"
//	#include "utils/debug/print_funcs.h"
//	#include "util/fsaccess.h"
//	#include "components/sd_mmc_mci.h"
//	#include "components/sd_mmc_mci_mem.h"
//	#include "config/conf_sd_mmc_mci.h"
//	#include "drivers/usb_task.h"
//	#include "usb/class/mass_storage/device_mass_storage_task.h"
//	#include "usb/class/mass_storage/host_mass_storage_task.h"
//	#include "usb/class/cdc/device_cdc_task.h"
//	#include "usb/class/cdc/host_cdc_task.h"
//	#include "drivers/spi.h"
//	#include "util/delay.h"
//	#include "drivers/smc.h"
//}

//#include "boards/board.h"
//#include "boards/backplane/dbg_led.h"
//#include "boards/backplane/MaxIoExpander.h"
//#include "boards/backplane/dbg_string.h"
//
//#include "HAL/HAL.h"
//#include "HAL/UART.h"
//#include "HAL/Interrupt.h"
//#include "HAL/Power.h"
//#include "HAL/RTC.h"
//#include "HAL/ADC.h"
//#include "HAL/GPIO.h"
//#include "HAL/GPS.h"
//
//#include "util/FileHandler.h"
//#include "util/crc.h"

#include "core/CommandMessage.h"
#include "core/DataMessage.h"
#include "core/ErrorMessage.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/SystemMode.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/ComMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/Dispatcher.h"

#include "util/Logger.h"

//#include "demos/BackplaneRev2CDRDemo.h"

using namespace std;
using namespace Phoenix::Core;
//using namespace Phoenix::HAL;
using namespace Phoenix::Servers;


#define DEBUG

#define ACS_EN 0
#define CDH_EN 1
#define CMD_EN 0
#define COM_EN 0
#define EPS_EN 0
#define ERR_EN 0
#define PLD_EN 0
#define THM_EN 0

#define ETH_EN 0
#define SPI_EN 0

//----------------------- Create server tasks -----------------------
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
	epsServer->SubsystemLoop();
	pthread_exit(NULL);

}

void * taskRunACS(void * params)
{
	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*acsServer);
	
	for(int i = 0; i < 15; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}
	
	logger->Log("Kicking off the ACS server", LOGGER_LEVEL_INFO);

	bool handlers = acsServer->RegisterHandlers();
	if(!handlers){
		logger->Log("ACS Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	acsServer->SubsystemLoop();
	pthread_exit(NULL);

}

void * taskRunPLD(void * params)
{
	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*pldServer);
	
//	for(int i = 0; i < 20; i++)
//	{
//		//wdm->Kick();
//		usleep(1000000);
//	}

	logger->Log("Kicking off the PLD server", LOGGER_LEVEL_INFO);

	bool handlers = pldServer->RegisterHandlers();
	if(!handlers){
		logger->Log("PLD Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	pldServer->SubsystemLoop();
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
	comServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunGPS(void * params)
{
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	//GPSInit();
	
// 	GPSData * gpsData = gpsServer->GetGPSDataPtr();
// 	gpsData->GPSWeek = 1000;
// 	gpsData->posX = 0;
// 	gpsData->posY = 0;
// 	gpsData->posZ = 0;
	
	
	modeManager->Attach(*gpsServer);
	
	for(int i = 0; i < 20; i++)
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

void * taskRunTHM(void * params)
{
	THMServer * thmServer = dynamic_cast<THMServer *> (Factory::GetInstance(THM_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*thmServer);
	//wdm->Kick();

	logger->Log("Kicking off the THM server", LOGGER_LEVEL_INFO);

	bool handlers = thmServer->RegisterHandlers();
	if(!handlers){
		logger->Log("THM Handlers registration failed!", LOGGER_LEVEL_ERROR);
	}
	thmServer->SubsystemLoop();
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
	
	for(int i = 0; i < 30; i++)
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
	
	for(int i = 0; i < 10; i++)
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
	cdhServer->SubsystemLoop();
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

	for(int i = 0; i < 10; i++)
	{
		//wdm->Kick();
		usleep(100000);
	}

	logger->Log("Kicking off the SPI HAL server", LOGGER_LEVEL_INFO);
	spi_server->SPI_HALServerLoop();
	pthread_exit(NULL);
}


int main(int argc, char * argv[])
{
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	Factory::GetInstance(ERROR_QUEUE_SINGLETON);
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	uint8 input = 1;
	//GetPin(EGSE_PRESENT, PIN_TYPE_GPIO, &input);
	if(input == 1)
	{
		//EGSE is plugged in, go into access mode
		modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);
		logger->Log("Access Mode Entered!", LOGGER_LEVEL_INFO);
	}
	else
	{
		//Flight go to startup mode
		modeManager->SetMode(MODE_STARTUP, LOCATION_ID_INVALID);
		logger->Log("Startup Mode Entered!", LOGGER_LEVEL_INFO);
	}

	logger->Log("Flight software initialization complete! Starting servers!", LOGGER_LEVEL_FATAL);

	bool threadCreated;

#if ACS_EN
	// ------------------------------------- ACS Thread -------------------------------------
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
	// ------------------------------------- EPS Thread -------------------------------------
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
	// ------------------------------------- PLD Thread -------------------------------------
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
	// ------------------------------------- ERR Thread -------------------------------------
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

#if THM_EN
	// ------------------------------------- THM Thread -------------------------------------
	pthread_t THMThread;
	threadCreated = pthread_create(&THMThread ,NULL,&taskRunTHM, NULL );
	if(!threadCreated)
	{
		logger->Log("THM Server Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("THM Server Thread Creation Failed!", LOGGER_LEVEL_FATAL);
	}
#endif //THM_EN

#if CMD_EN
	// ------------------------------------- CMD Thread -------------------------------------
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
	// ------------------------------------- CDH Thread -------------------------------------
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
	// ------------------------------------- ETH Thread -------------------------------------
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
	// ------------------------------------- SPI Thread -------------------------------------
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

	//CREATE_TASK(taskRunSCH, (const signed char* const)"SCH task", 5000, NULL, 0, NULL);
	//CREATE_TASK(taskRunCMD, (const signed char* const)"CMD task", 5000, NULL, 0, NULL);

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

#if THM_EN
	pthread_join(THMThread, NULL);
#endif //THM_EN

#if PLD_EN
	pthread_join(PLDThread, NULL);
#endif //PLD_EN

#if CDH_EN
	pthread_join(CDHThread, NULL);
#endif //CDH_EN

	logger->Log("Flight Software exiting from main!", LOGGER_LEVEL_FATAL);

	return 42;
}
