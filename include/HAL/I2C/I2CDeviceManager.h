/*
 * I2CDeviceManager.h
 *
 *  Created on: June 2, 2016
 *      Author: Alex St. Clair
 */

#ifndef I2CDEVICEMANAGER_H_
#define I2CDEVICEMANAGER_H_

#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"

namespace Phoenix
{
	namespace HAL
	{
		class I2CDeviceManager{
		public:
			I2CDeviceManager();
			bool initializeHS(void);
			void initializePM(void);
			void startPMMeas(void);
			void getHSStatus(float *);
			void getPMStatus(PowerMonitor_Data * data);
		private:
			// Hot swaps
			HotSwap * hotSwaps[16];
			const static HotSwap_Address addressesHS[16];
			const static HotSwap_Fault faultsHS[16];
			const static float resistorsHS[16];

			// Power monitors
			PowerMonitor * powerMonitors[4];
			const static PowerMonitor_Address addressesPM[4];
		};
	}
}


#endif