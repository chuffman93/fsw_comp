/*! \file HotSwaps.h
 *  \brief Define methods and functionality to read from hotswaps
 *
 * Robert Belter Summer 2016
 *
 */
#include <stdint.h>
#include "HAL/I2C/I2C_Device.h"

#ifndef HOT_SWAPS_H_
#define HOT_SWAPS_H_

#define IOtoInt(port, val)  (port - 'A')*32 + val
#define IOPort(io) (io/32) + 'A'
#define IOPin(io) io%32

namespace AllStar
{
	namespace HAL
	{

		typedef enum {
			//COM Monitors
			COM_3V3_ADDR					 =0x80,
			COM_VBAT_ADDR					 =0x82,
			COM_12V0_ADDR					 =0x84,
			//ACS Monitors
			ACS_3V3_ADDR					 =0x86,
			ACS_VBAT_ADDR					 =0x88,
			ACS_12V0_ADDR					 =0x8A,
			//PROP Monitors
			PROP_3V3_ADDR					 =0x8C,
			PROP_VBAT_ADDR					 =0x8E,
			PROP_12V0_ADDR					 =0x90,
			//PLD Monitors
			PLD_3V3_ADDR					 =0x92,
			PLD_VBAT_ADDR					 =0x94,
			PLD_12V0_ADDR					 =0x96,
			//GPS Monitors
			GPS_3V3_ADDR					 =0x98,
			GPS_VBAT_ADDR					 =0x9A,
			//AUXCOM Monitors
			AUXCOM_3V3_ADDR					 =0x9C,
			AUXCOM_VBAT_ADDR				 =0x9E,
		}HotSwap_Address;

		typedef enum {
			//COM Monitors
			COM_3V3_FAULT					 =IOtoInt('B', 13),
			COM_VBAT_FAULT					 =IOtoInt('B', 7),
			COM_12V0_FAULT					 =IOtoInt('D', 19),
			//ACS Monitors
			ACS_3V3_FAULT					 =IOtoInt('B', 5),
			ACS_VBAT_FAULT					 =IOtoInt('B', 2),
			ACS_12V0_FAULT					 =IOtoInt('B', 0),
			//PROP Monitors
			PROP_3V3_FAULT					 =IOtoInt('B', 8),
			PROP_VBAT_FAULT					 =IOtoInt('B', 16),
			PROP_12V0_FAULT					 =IOtoInt('B', 6),
			//PLD Monitors
			PLD_3V3_FAULT					 =IOtoInt('B', 14),
			PLD_VBAT_FAULT					 =IOtoInt('D', 25),
			PLD_12V0_FAULT					 =IOtoInt('B', 12),
			//GPS Monitors
			GPS_3V3_FAULT					 =IOtoInt('D', 15),
			GPS_VBAT_FAULT					 =IOtoInt('D', 5),
			//AUXCOM Monitors
			AUXCOM_3V3_FAULT				 =IOtoInt('B', 9),
			AUXCOM_VBAT_FAULT				 =IOtoInt('D', 24),
		}HotSwap_Fault;

		class HotSwap : public I2C_Device{
		public:
			HotSwap(HotSwap_Address addr, HotSwap_Fault faultIO, float SenseResistor);

			//Init MUST be called before any other function
			bool Init();

			bool Status(float *Voltage, float *Current);

			bool Fault();

		private:
			int fault_fd;
			float SenseResistorValue;
			HotSwap_Fault faultLine;
		};
	}
}

#endif
