/*! \file HotSwaps.cpp
 *  \brief Define methods and functionality to read from hotswaps
 *
 * Robert Belter Summer 2016
 *
 */
#include "HAL/I2C/HotSwaps.h"
#include "util/Logger.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace AllStar::Core;
Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

namespace AllStar
{
	namespace HAL
	{
		HotSwap::HotSwap(HotSwap_Address addr, HotSwap_Fault faultIO, float SenseResistor)
				:I2C_Device(2, addr), faultLine(faultIO), SenseResistorValue(SenseResistor)
		{
			// Intentionally left empty
		}

		bool HotSwap::Init(){
			//Initialize the fault line GPIO
			logger->Log("HotSwap: Initializing", LOGGER_LEVEL_DEBUG);
			char cmdString[100];
			sprintf(cmdString, "echo \"%d\" > /sys/class/gpio/export", faultLine);
			system(cmdString);

			sprintf(cmdString, "echo \"in\" > /sys/class/gpio/pio%c%d/direction", IOPort(faultLine), IOPin(faultLine));
			system(cmdString);

			sprintf(cmdString, "/sys/class/gpio/pio%c%d/value", IOPort(faultLine), IOPin(faultLine));
			fault_fd = open(cmdString, O_RDONLY);

			//Write to the configuration register
			return I2C_write(0x05); //Set to do voltage and current continuously
		}

		void HotSwap::Status(float *Voltage, float *Current){
			uint8_t buf[3];
			bool read = I2C_read(buf, 3);
			if(read){
				uint16_t voltRaw = (buf[0]<<4) | (0x0F & (buf[2]>>4));
				uint16_t currRaw = (buf[1]<<4) | (0x0F & buf[2]);

				*Voltage = 26.35 * ((float)voltRaw/4096); //Volts
				*Current = 0.10584 * ((float)currRaw/4096) / SenseResistorValue; //Current
				logger->Log("    HotSwap Voltage: %f", *Voltage, LOGGER_LEVEL_DEBUG);
				logger->Log("    HotSwap Current: %f", *Current , LOGGER_LEVEL_DEBUG);
			}else{
				*Voltage = -1001;
				*Current = -1001;
				logger->Log("----HotSwaps: Error reading hot swap!", LOGGER_LEVEL_ERROR);
			}
		}

		bool HotSwap::Fault(){
			logger->Log("HotSwaps: Checking fault", LOGGER_LEVEL_DEBUG);

			uint8_t readVal = 0;
			read(fault_fd, &readVal, 1);
			return !(readVal == 0);
		}
	}
}
