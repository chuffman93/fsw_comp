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
#include "servers/ErrorOctopus.h"
#include "servers/ErrorQueue.h"

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

//#include "demos/BackplaneRev2CDRDemo.h"

using namespace std;
using namespace Phoenix::Core;
//using namespace Phoenix::HAL;
using namespace Phoenix::Servers;


#define DEBUG

/*! \name USART Settings
*/
//! @{

//#define MAP_SIZE(map)	sizeof(map)/sizeof(map[0])
//
//void*   __dso_handle = (void*) &__dso_handle;
//
//extern "C"
//{
//	void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
//	{
//		while (1)
//		{
//			gpio_local_tgl_gpio_pin(AVR32_PIN_PX07);
//		}
//	}
//}
//
//struct mapType
//{
//	void * p;
//	size_t size;
//	bool used;
//};
//
//#define MAX_ENTRIES 1000
//#define PROFILING_PIN	AVR32_PIN_PX07
//
//static void PrintHex(unsigned long n, uint8 hexDigits);
//
//#define SET_POWER_STATE		0x01
//
//void init_spiMaster(volatile avr32_spi_t *spi, long cpuHz)
//{
//	//  volatile avr32_pio_t *pioc = &AVR32_PIOC;
//	spi_options_t spiOptions;
//	uint8_t delay = 31;
//
//	spiOptions.reg = 0;
//	spiOptions.baudrate = 8000000;
//	spiOptions.bits = 8;
//	spiOptions.spck_delay = 0;
//	spiOptions.trans_delay = delay;
//	spiOptions.stay_act = 0;
//	spiOptions.spi_mode = 0;
//	spiOptions.modfdis = 1;
//
//	/* Initialize as master */
//	spi_initMaster(spi, &spiOptions);
//
//	/* Set master mode; variable_ps, pcs_decode, delay */
//	spi_selectionMode(spi, 1, 1, 0);
//
//	spi_setupChipReg(spi, &spiOptions, cpuHz);
//	spiOptions.reg = 1;
//	spi_setupChipReg(spi, &spiOptions, cpuHz);
//	spiOptions.reg = 2;
//	spi_setupChipReg(spi, &spiOptions, cpuHz);
//	spiOptions.reg = 3;
//	spi_setupChipReg(spi, &spiOptions, cpuHz);
//
//	/* Select slave chip 0 (SPI_NPCS0) */
//	//	spi_selectChip(spi, 0);
//	//	spi_unselectChip(spi, 0);
//
//	spi_enable(spi);
//
//	//  pioc->sodr = (1<<AVR32_PIO_P7)|(1<<AVR32_PIO_P0);
//	//  pioc->codr = (1<<AVR32_PIO_P6)|(1<<AVR32_PIO_P1);
//	//  rgb_setColor(pioc, 0);
//}
//
//void spi_masterSend(volatile avr32_spi_t *spi, char *string)
//{
//	//  volatile avr32_pio_t *pioc = &AVR32_PIOC;
//	int error;
//	char *textStringPtr = string;
//
//	//  pioc->codr = (1<<AVR32_PIO_P0);
//	//  pioc->sodr = (1<<AVR32_PIO_P1);
//
//	do {
//		error = spi_write(spi, (unsigned short) *textStringPtr);
//		delay_ms(1);
//	} while (*textStringPtr++ != '\n');
//
//	//  pioc->codr = (1<<AVR32_PIO_P1);
//	//  pioc->sodr = (1<<AVR32_PIO_P0);
//
//	if (error != SPI_OK) {
//	}
//}
//
//#define FS_DEMO 0
//volatile bool interrupted = false;
//
//__attribute__((__interrupt__)) static void subsystemInterruptHandler(void)
//{
//	gpio_clear_pin_interrupt_flag(AVR32_PIN_PX25);
//	interrupted = true;
//}
//
//#define PLD_ADC_CONVST	AVR32_PIN_PX46
//#define PLD_ADC_EOC		AVR32_PIN_PX45
//#define PLD_ADC_CS		AVR32_PIN_PA01
//
//#define PWR_ADC_CS		AVR32_PIN_PA05
//#define PWR_ADC_CS2		AVR32_PIN_PA06
//
//static const char HEX_DIGITS[] = "0123456789ABCDEF";
//void PrintHex(unsigned long n, uint8 hexDigits)
//{
//	char tmp[8];
//	int i;
//
//	if (hexDigits > 8)
//	{
//		return;
//	}
//
//	// Convert the given number to an ASCII hexadecimal representation.
//	for (i = hexDigits-1; i >= 0; i--)
//	{
//		tmp[i] = HEX_DIGITS[n & 0xF];
//		n >>= 4;
//	}
//
//	// Transmit the resulting string with the given USART.
//	int written = 0;
//	while (written < hexDigits)
//	{
//		written += Phoenix::HAL::UARTWrite(0, (const uint8 *)(tmp+written), hexDigits);
//		vTaskDelay(1);
//	}
//}
//
//void print_buffer_hex(unsigned char * buffer_in, uint32 size)
//{
//	for (size_t i = 0; i < size; ++i)
//	{
//		/*if (i % 16 == 0)
//		{
//		print_dbg_short_hex(i);
//		usart_write_line(UART_0, ": ");
//		}
//		print_dbg_char_hex(buffer_in[i]);
//		usart_bw_write_char(UART_0, ' ');
//		if ((i+1) % 16 == 0)
//		{
//		usart_write_line(UART_0, "\n");
//		}*/
//		print_dbg_char(buffer_in[i]);
//	}
//}
//
//static xTaskHandle ucTaskHandle;
//static xTaskHandle * curTask = NULL;
//static xSemaphoreHandle taskSem;
//static xSemaphoreHandle taskDoneSem;
//int openFd;
//
//// Create server tasks


//TODO:Add meaningful exit information to each server pthread_exit
void * taskRunEPS(void * params)
{
	//EnablePinInterrupt(INTERRUPT_PIN_EPS, INTERRUPT_EDGE_RISING);

	EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*epsServer);

	epsServer->RegisterHandlers();
	epsServer->SubsystemLoop();
	pthread_exit(NULL);

}

void * taskRunACS(void * params)
{
	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*acsServer);
	
	for(int i = 0; i < 15; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}
	
	acsServer->RegisterHandlers();
	acsServer->SubsystemLoop();
	pthread_exit(NULL);

}

void * taskRunPLD(void * params)
{
	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*pldServer);
	
//	for(int i = 0; i < 20; i++)
//	{
//		//wdm->Kick();
//		usleep(1000000);
//	}

	pldServer->RegisterHandlers();
	pldServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunCOM(void * params)
{
	COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*comServer);
		
	for(int i = 0; i < 25; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}
	
	comServer->RegisterHandlers();
	comServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunGPS(void * params)
{
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
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

	gpsServer->RegisterHandlers();
	gpsServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunTHM(void * params)
{
	THMServer * thmServer = dynamic_cast<THMServer *> (Factory::GetInstance(THM_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*thmServer);
	//wdm->Kick();

	thmServer->RegisterHandlers();
	thmServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunERR(void * params)
{
	ErrorOctopus * errServer = dynamic_cast<ErrorOctopus *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*errServer);
	//wdm->Kick();

	errServer->RegisterHandlers();
	errServer->SubsystemLoop();
	pthread_exit(NULL);
}

void * taskRunSCH(void * params)
{
	SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*schServer);
	
	for(int i = 0; i < 30; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}
	
	schServer->RegisterHandlers();
	schServer->SubsystemLoop();
	pthread_exit(NULL);
}

void taskRunCMD(void * params)
{
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	
	modeManager->Attach(*cmdServer);
	
	for(int i = 0; i < 40; i++)
	{
		//wdm->Kick();
		usleep(1000000);
	}

	cmdServer->SubsystemLoop();
	pthread_exit(NULL);
}


/*! \name Hmatrix bus configuration*/

/*void init_hmatrix(void)
{
	union
	{
		unsigned long                 scfg;
		avr32_hmatrix_scfg_t          SCFG;
	} u_avr32_hmatrix_scfg;

	// For the internal-flash HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;
}

//__attribute__((__section__(".mram"))) static volatile unsigned char mram_data[0x80000];
//static volatile unsigned char __attribute__((__section__(".mram"))) thisBetterWork;
static volatile unsigned char * tBW2 = (unsigned char *)(0xd0000000);*/

/*! \brief This is an example demonstrating the USART RS232 TX and RX
*         functionalities using the USART driver.
*/



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


	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	Factory::GetInstance(ERROR_QUEUE_SINGLETON);
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	uint8 input = 1;
	//GetPin(EGSE_PRESENT, PIN_TYPE_GPIO, &input);
	if(input == 1)
	{
		//EGSE is plugged in, go into access mode
		modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);

	}
	else
	{
		//Flight go to startup mode
		modeManager->SetMode(MODE_STARTUP, LOCATION_ID_INVALID);

	}
	printf("Flight Software Initialization Complete!\n");
//
	bool threadCreated;
	//CREATE_TASK(taskRunACS, (const signed char* const)"ACS task", 2000, NULL, 0, NULL);
	pthread_t ACSThread;
	//threadCreated = pthread_create(&ACSThread, NULL, &taskRunACS, NULL);
#ifdef DEBUG
	if(!threadCreated)
	{
		printf("ACS Server Thread Creation Success\n");
	}
	else
	{
		printf("ACS Server Thread Creation Failed\n");
	}
#endif //DEBUG

	//CREATE_TASK(taskRunEPS, (const signed char* const)"EPS task", 2000, NULL, 0, NULL);
	pthread_t EPSThread;
	threadCreated = pthread_create(&EPSThread, NULL, &taskRunEPS, NULL);

//#ifdef DEBUG
 	if(!threadCreated)
	{
		printf("EPS Server Thread Creation Success\n");
	}
	else
	{
		printf("EPS Server Thread Creation Failed\n");
	}
//#endif //DEBUG

	//CREATE_TASK(taskRunCOM, (const signed char* const)"COM task", 5000, NULL, 0, NULL);
	//CREATE_TASK(taskRunPLD, (const signed char* const)"PLD task", 2000, NULL, 0, NULL);

	pthread_t PLDThread;
	//threadCreated = pthread_create(&PLDThread, NULL, &taskRunPLD, NULL);
#ifdef DEBUG
	if(!threadCreated)
	{
		printf("PLD Server Thread Creation Success\n");
	}
	else
	{
		printf("PLD Server Thread Creation Failed\n");
	}
#endif //DEBUG
	//CREATE_TASK(taskRunERR, (const signed char* const)"ERR task", 2000, NULL, 0, NULL);
	pthread_t ERRThread;
	threadCreated = pthread_create(&ERRThread, NULL, &taskRunERR, NULL);
//#ifdef DEBUG
	if(!threadCreated)
	{
		printf("ERR Server Thread Creation Success\n");
	}
	else
	{
		printf("ERR Server Thread Creation Failed\n");
	}
//#endif //DEBUG
	//CREATE_TASK(taskRunGPS, (const signed char* const)"GPS task", 5000, NULL, 0, NULL);
	//CREATE_TASK(taskRunTHM, (const signed char* const)"THM task", 2000, NULL, 0, NULL);
	pthread_t THMThread;
	threadCreated = pthread_create(&THMThread ,NULL,&taskRunTHM, NULL );
//#ifdef DEBUG
	if(!threadCreated)
	{
		printf("THM Server Thread Creation Success\n");
	}
	else
	{
		printf("THM Server Thread Creation Failed\n");
	}
//#endif //DEBUG

	//CREATE_TASK(taskRunSCH, (const signed char* const)"SCH task", 5000, NULL, 0, NULL);
	//CREATE_TASK(taskRunCMD, (const signed char* const)"CMD task", 5000, NULL, 0, NULL);

	//vTaskStartScheduler();
	//portDBG_TRACE("FreeRTOS returned.");

	//pthread_join(ACSThread, NULL);
	pthread_join(EPSThread, NULL);
	//pthread_join(ERRThread, NULL);
	//pthread_join(THMThread, NULL);
	//pthread_join(PLDThread, NULL);
	return 42;
}
