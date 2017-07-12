/*! \file PowerMonitor.cpp
 *  \brief Define methods and functionality to read from power monitor
 *
 * Robert Belter Summer 2016
 *
 */
#include <fcntl.h>
#include "HAL/I2C/PowerMonitor.h"
#include "util/Logger.h"

namespace AllStar
{
	namespace HAL
	{
		PowerMonitor::PowerMonitor(PowerMonitor_Address addr)
				:I2C_Device(2, addr)
		{
			// Intentionally left empty
		}

		void PowerMonitor::Init(PowerMonitor_Config *config){
			Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Debug("PowerMonitor: Initializing");

			//Write to the configuration register
			uint8_t buf[2];
			buf[0] = PWRMON_CONTROL;
			buf[1] = 0x05; //Cont. Operation, monitor sense_p not VCC
			I2C_write(buf, 2);

			uint32_t raw;
			//Set up all of the max and min thresholds
			raw = (uint32_t) ( config->MaxPower/(25.0/1000000.0)/(1/SenseResistorValue)/(25.0/1000.0) ); //25uV *1/SenseResister * 25mV to convert from Watts
			I2C_writeReg(PWRMON_MAXTHPOWMSB2, 0xFF & (raw >> 16));
			I2C_writeReg(PWRMON_MAXTHPOWMSB1, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MAXTHPOWLSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MinPower/(25.0/1000000.0)/(1/SenseResistorValue)/(25.0/1000.0) ); //25uV *1/SenseResister * 25mV to convert from Watts
			I2C_writeReg(PWRMON_MINTHPOWMSB2, 0xFF & (raw >> 16));
			I2C_writeReg(PWRMON_MINTHPOWMSB1, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MINTHPOWLSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MaxCurrent/(25.0/1000000.0)/(1/SenseResistorValue) ); //25uV *1/SenseResister to convert from A
			I2C_writeReg(PWRMON_MAXTHSENSEMSB, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MAXTHSENSELSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MinCurrent/(25.0/1000000.0)/(1/SenseResistorValue) ); //25uV *1/SenseResister to convert from A
			I2C_writeReg(PWRMON_MINTHSENSEMSB, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MINTHSENSELSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MaxVoltage/(25.0/1000.0) ); //25mV to convert from V
			I2C_writeReg(PWRMON_MAXTHVINMSB, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MAXTHVINLSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MinVoltage/(25.0/1000.0) ); //25mV to convert from V
			I2C_writeReg(PWRMON_MINTHVINMSB, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MINTHVINLSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MaxADIN/(0.5/1000.0) ); //0.5mV to convert from V
			I2C_writeReg(PWRMON_MAXTHADINMSB, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MAXTHADINLSB, 0xFF & (raw >> 0));

			raw = (uint32_t) ( config->MinADIN/(0.5/1000.0) ); //0.5mV to convert from V
			I2C_writeReg(PWRMON_MINTHADINMSB, 0xFF & (raw >> 8));
			I2C_writeReg(PWRMON_MINTHADINLSB, 0xFF & (raw >> 0));

		}

		void PowerMonitor::StartMeasurement(){
			Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Debug("PowerMonitor: StartMeasurement()");

			I2C_writeReg(PWRMON_CONTROL, I2C_readReg(PWRMON_CONTROL) | 0x10);

			I2C_writeReg(PWRMON_MAXPOWMSB2, 0xFF);
			I2C_writeReg(PWRMON_MAXPOWMSB1, 0xFF);
			I2C_writeReg(PWRMON_MAXPOWLSB, 0xFF);
			I2C_writeReg(PWRMON_MINPOWMSB2, 0x00);
			I2C_writeReg(PWRMON_MINPOWMSB1, 0x00);
			I2C_writeReg(PWRMON_MINPOWLSB, 0x00);

			I2C_writeReg(PWRMON_MAXSENSEMSB, 0xFF);
			I2C_writeReg(PWRMON_MAXSENSELSB, 0xFF);
			I2C_writeReg(PWRMON_MINSENSEMSB, 0x00);
			I2C_writeReg(PWRMON_MINSENSELSB, 0x00);

			I2C_writeReg(PWRMON_MAXVINMSB, 0xFF);
			I2C_writeReg(PWRMON_MAXVINLSB, 0xFF);
			I2C_writeReg(PWRMON_MINVINMSB, 0x00);
			I2C_writeReg(PWRMON_MINVINLSB, 0x00);

			I2C_writeReg(PWRMON_MAXADINMSB, 0xFF);
			I2C_writeReg(PWRMON_MAXADINLSB, 0xFF);
			I2C_writeReg(PWRMON_MINADINMSB, 0x00);
			I2C_writeReg(PWRMON_MINADINLSB, 0x00);

			I2C_writeReg(PWRMON_CONTROL, I2C_readReg(PWRMON_CONTROL) & ~0x10);
		}

		void PowerMonitor::Status(PowerMonitor_Data *data){
			Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Debug("PowerMonitor: Status()");

			uint32_t raw;
			//Read values for Power
			raw = (I2C_readReg(PWRMON_MAXPOWMSB2) << 16) | (I2C_readReg(PWRMON_MAXPOWMSB1) << 8) | I2C_readReg(PWRMON_MAXPOWLSB);
			data->MaxPower =  (float)raw *(25.0/1000000.0)*(1/SenseResistorValue)*(25.0/1000.0); //25uV *1/SenseResister * 25mV to convert to Watts
			raw = (I2C_readReg(PWRMON_MINPOWMSB2) << 16) | (I2C_readReg(PWRMON_MINPOWMSB1) << 8) | I2C_readReg(PWRMON_MINPOWLSB);
			data->MinPower =  (float)raw *(25.0/1000000.0)*(1/SenseResistorValue)*(25.0/1000.0); //25uV *1/SenseResister * 25mV to convert to Watts
			raw = (I2C_readReg(PWRMON_POWERMSB2) << 16) | (I2C_readReg(PWRMON_POWERMSB1) << 8) | I2C_readReg(PWRMON_POWERLSB);
			data->CurrPower =  (float)raw *(25.0/1000000.0)*(1/SenseResistorValue)*(25.0/1000.0); //25uV *1/SenseResister * 25mV to convert to Watts

			//Read values for current
			raw = (I2C_readReg(PWRMON_MAXSENSEMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_MAXSENSELSB) >> 4));
			data->MaxCurrent = (float)raw *(25.0/1000000.0)*(1/SenseResistorValue); //25uV * 1/SenseResistor to convert to A
			raw = (I2C_readReg(PWRMON_MINSENSEMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_MINSENSELSB) >> 4));
			data->MinCurrent = (float)raw *(25.0/1000000.0)*(1/SenseResistorValue); //25uV * 1/SenseResistor to convert to A
			raw = (I2C_readReg(PWRMON_SENSEMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_SENSELSB) >> 4));
			data->CurrCurrent = (float)raw *(25.0/1000000.0)*(1/SenseResistorValue); //25uV * 1/SenseResistor to convert to A

			//Read values for voltage
			raw = (I2C_readReg(PWRMON_MAXVINMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_MAXVINLSB) >> 4));
			data->MaxVoltage = (float)raw * (25.0/1000.0); //25mV to convert to V
			raw = (I2C_readReg(PWRMON_MINVINMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_MINVINLSB) >> 4));
			data->MinVoltage = (float)raw * (25.0/1000.0); //25mV to convert to V
			raw = (I2C_readReg(PWRMON_VINMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_VINLSB) >> 4));
			data->CurrVoltage = (float)raw * (25.0/1000.0); //25mV to convert to V

			//Read values for aux voltage
			raw = (I2C_readReg(PWRMON_MAXADINMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_MAXADINLSB) >> 4));
			data->MaxADIN = (float)raw * (0.5/1000.0); //0.5mV to convert to V
			raw = (I2C_readReg(PWRMON_MINADINMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_MINADINLSB) >> 4));
			data->MinADIN = (float)raw * (0.5/1000.0); //0.5mV to convert to V
			raw = (I2C_readReg(PWRMON_ADINMSB) << 4) | (0x0F & (I2C_readReg(PWRMON_ADINLSB) >> 4));
			data->CurrADIN = (float)raw * (0.5/1000.0); //0.5mV to convert to V

			//read faults
			raw = I2C_readReg(PWRMON_FAULTCOR);
			data->MaxPowerFault 	= 0x01 & (raw >> 7);
			data->MinPowerFault 	= 0x01 & (raw >> 6);
			data->MaxCurrentFault 	= 0x01 & (raw >> 5);
			data->MinCurrentFault 	= 0x01 & (raw >> 4);
			data->MaxVoltageFault 	= 0x01 & (raw >> 3);
			data->MinVoltageFault 	= 0x01 & (raw >> 2);
			data->MaxADINFault 		= 0x01 & (raw >> 1);
			data->MinADINFault		= 0x01 & (raw >> 0);
		}

	}
}
