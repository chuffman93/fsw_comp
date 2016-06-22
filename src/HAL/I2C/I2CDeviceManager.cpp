#include "HAL/I2C/I2CDeviceManager.h"
#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"
#include "core/StdTypes.h"

namespace Phoenix
{
	namespace HAL
	{
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Hot Swap Information ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		const HotSwap_Address I2CDeviceManager::addressesHS[16] = {
				COM_3V3_ADDR,COM_VBAT_ADDR,COM_12V0_ADDR,  		//COM:		0-2
				ACS_3V3_ADDR,ACS_VBAT_ADDR,ACS_12V0_ADDR, 		//ACS:		3-5
				PROP_3V3_ADDR,PROP_VBAT_ADDR,PROP_12V0_ADDR, 	//PROP:		6-8
				PLD_3V3_ADDR,PLD_VBAT_ADDR,PLD_12V0_ADDR, 		//PLD:		9-11
				GPS_3V3_ADDR,GPS_VBAT_ADDR, 					//GPS:		12-13
				AUXCOM_3V3_ADDR,AUXCOM_VBAT_ADDR				//AUXCOM:	14-15
		};

		const HotSwap_Fault I2CDeviceManager::faultsHS[16] = {
				COM_3V3_FAULT,COM_VBAT_FAULT,COM_12V0_FAULT,	//COM:		0-2
				ACS_3V3_FAULT,ACS_VBAT_FAULT,ACS_12V0_FAULT,	//ACS:		3-5
				PROP_3V3_FAULT,PROP_VBAT_FAULT,PROP_12V0_FAULT,	//PROP:		6-8
				PLD_3V3_FAULT,PLD_VBAT_FAULT,PLD_12V0_FAULT,	//PLD:		9-11
				GPS_3V3_FAULT,GPS_VBAT_FAULT,					//GPS:		12-13
				AUXCOM_3V3_FAULT,AUXCOM_VBAT_FAULT				//AUXCOM:	14-15
		};

		const float I2CDeviceManager::resistorsHS[16] = {
				0.01,0.01,0.015,	//COM:		0-2
				0.01,0.01,0.015,	//ACS:		3-5
				0.01,0.01,0.015,	//PROP:		6-8
				0.01,0.01,0.015,	//PLD:		9-11
				0.01,0.01,			//GPS:		12-13
				0.01,0.01			//AUXCOM:	14-15
		};

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Power Monitor Information ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		const PowerMonitor_Address I2CDeviceManager::addressesPM[4] = {A5_1V8_ADDR,A5_1V2_ADDR,ETH_2V5_ADDR,ETH_1V2_ADDR};

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		I2CDeviceManager::I2CDeviceManager(){
			//Intentionally left blank
		}

		I2CDeviceManager::~I2CDeviceManager(){
			for(uint8 i = 0; i < 16; i++){
				if(i < 4){
					delete powerMonitors[i];
				}
				delete hotSwaps[i];
			}
		}

		bool I2CDeviceManager::initializeHS(void){
			bool success = true;
			for(uint8 i = 0; i < 16; i++){
				hotSwaps[i] = new HotSwap(addressesHS[i],faultsHS[i],resistorsHS[i]);
				success &= hotSwaps[i]->Init();
			}
			return success;
		}

		void I2CDeviceManager::initializePM(){
			for(uint8 i = 0; i < 4; i++){
				powerMonitors[i] = new PowerMonitor(addressesPM[i]);
			}
		}

		void I2CDeviceManager::startPMMeas(void){
			for(uint8 i = 0; i < 4; i++){
				powerMonitors[i]->StartMeasurement();
			}
		}

		void I2CDeviceManager::getHSStatus(float * status){
			for(uint8 i = 0; i < 16; i++){
				hotSwaps[i]->Status(&status[i], &status[i+16]);
			}
		}

		void I2CDeviceManager::getPMStatus(PowerMonitor_Data * data){
			for(uint8 i = 0; i < 4; i++){
				powerMonitors[i]->Status(&data[i]);
			}
		}
	}
}
