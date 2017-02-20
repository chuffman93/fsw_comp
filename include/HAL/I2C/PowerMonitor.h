/*! \file PowerMonitor.h
 *  \brief Define methods and functionality to read from power monitor
 *
 * Robert Belter Summer 2016
 *
 */
#include <stdint.h>
#include "I2C_Device.h"

#ifndef POWERMONITOR_H_
#define POWERMONITOR_H_

namespace AllStar
	{
	namespace HAL
		{
		typedef enum {
			A5_1V8_ADDR 			=0xCE,
			A5_1V2_ADDR 			=0xD0,
			ETH_2V5_ADDR 			=0xD2,
			ETH_1V2_ADDR 			=0xD4,
		}PowerMonitor_Address;

		typedef struct {
			float MaxPower;
			float MinPower;
			float MinVoltage;
			float MaxVoltage;
			float MinADIN;
			float MaxADIN;
			float MinCurrent;
			float MaxCurrent;
		}PowerMonitor_Config;

		typedef struct {
			float MaxPower;
			float MinPower;
			float CurrPower;
			bool MinPowerFault;
			bool MaxPowerFault;

			float MaxVoltage;
			float MinVoltage;
			float CurrVoltage;
			bool MinVoltageFault;
			bool MaxVoltageFault;

			float MinADIN;
			float MaxADIN;
			float CurrADIN;
			bool MinADINFault;
			bool MaxADINFault;

			float MaxCurrent;
			float MinCurrent;
			float CurrCurrent;
			bool MinCurrentFault;
			bool MaxCurrentFault;
		}PowerMonitor_Data;

		class PowerMonitor : public I2C_Device{
		public:
			PowerMonitor(PowerMonitor_Address addr);

			//Init MUST be called before any other function
			void Init(PowerMonitor_Config *config);

			void Status(PowerMonitor_Data *data);
			void StartMeasurement();

		private:
			float SenseResistorValue;//TODO Fix = 0.02;

			typedef enum{
				PWRMON_CONTROL				=0x00,
				PWRMON_STATUS				=0x02,
				PWRMON_FAULT				=0x03,
				PWRMON_FAULTCOR				=0x04,
				//Power
				PWRMON_POWERMSB2			=0x05,
				PWRMON_POWERMSB1			=0x06,
				PWRMON_POWERLSB				=0x07,
				PWRMON_MAXPOWMSB2			=0x08,
				PWRMON_MAXPOWMSB1			=0x09,
				PWRMON_MAXPOWLSB			=0x0A,
				PWRMON_MINPOWMSB2			=0x0B,
				PWRMON_MINPOWMSB1			=0x0C,
				PWRMON_MINPOWLSB			=0x0D,

				PWRMON_MAXTHPOWMSB2			=0x0E,
				PWRMON_MAXTHPOWMSB1			=0x0F,
				PWRMON_MAXTHPOWLSB			=0x10,
				PWRMON_MINTHPOWMSB2			=0x11,
				PWRMON_MINTHPOWMSB1			=0x12,
				PWRMON_MINTHPOWLSB			=0x13,
				//Current
				PWRMON_SENSEMSB				=0x14,
				PWRMON_SENSELSB				=0x15,
				PWRMON_MAXSENSEMSB			=0x16,
				PWRMON_MAXSENSELSB			=0x17,
				PWRMON_MINSENSEMSB			=0x18,
				PWRMON_MINSENSELSB			=0x19,


				PWRMON_MAXTHSENSEMSB		=0x1A,
				PWRMON_MAXTHSENSELSB		=0x1B,
				PWRMON_MINTHSENSEMSB		=0x1C,
				PWRMON_MINTHSENSELSB		=0x1D,

				//Voltage
				PWRMON_VINMSB				=0x1E,
				PWRMON_VINLSB				=0x1F,
				PWRMON_MAXVINMSB			=0x20,
				PWRMON_MAXVINLSB			=0x21,
				PWRMON_MINVINMSB			=0x22,
				PWRMON_MINVINLSB			=0x23,


				PWRMON_MAXTHVINMSB			=0x24,
				PWRMON_MAXTHVINLSB			=0x25,
				PWRMON_MINTHVINMSB			=0x26,
				PWRMON_MINTHVINLSB			=0x27,
				//Another Voltage
				PWRMON_ADINMSB				=0x28,
				PWRMON_ADINLSB				=0x29,
				PWRMON_MAXADINMSB			=0x2A,
				PWRMON_MAXADINLSB			=0x2B,
				PWRMON_MINADINMSB			=0x2C,
				PWRMON_MINADINLSB			=0x2D,


				PWRMON_MAXTHADINMSB			=0x2E,
				PWRMON_MAXTHADINLSB			=0x2F,
				PWRMON_MINTHADINMSB			=0x30,
				PWRMON_MINTHADINLSB			=0x31,
			}PowerMonitor_Register;

		};
	}
}

#endif
