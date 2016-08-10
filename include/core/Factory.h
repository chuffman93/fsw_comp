/*
* Factory.h
*
* Created: 8/22/2013 11:24:40 AM
*  Author: Admin
*/


#ifndef FACTORY_H_
#define FACTORY_H_

#include "core/Singleton.h"

namespace AllStar
{
	namespace Core
	{
		enum SingletonType
		{
			SINGLETON_TYPE_START,
			DISPATCHER_SINGLETON = SINGLETON_TYPE_START,
			MODE_MANAGER_SINGLETON,
			WATCHDOG_MANAGER_SINGLETON,
			ERROR_QUEUE_SINGLETON,
			FILE_HANDLER_SINGLETON,
			LOGGER_SINGLETON,
			ACS_SERVER_SINGLETON,
			CMD_SERVER_SINGLETON,
			COM_SERVER_SINGLETON,
			EPS_SERVER_SINGLETON,
			ERR_SERVER_SINGLETON,
			GPS_SERVER_SINGLETON,
			PLD_SERVER_SINGLETON,
			SCH_SERVER_SINGLETON,
			THM_SERVER_SINGLETON,
			CDH_SERVER_SINGLETON,
			ETH_HALSERVER_SINGLETON,
			SPI_HALSERVER_SINGLETON,
#ifdef WIN32
			RF_INTERFACE_SINGLETON,
			GROUND_COM_SERVER_SINGLETON,
			FLIGHT_COM_MOCK_HARDWARE_SINGLETON,
			GROUND_COM_MOCK_HARDWARE_SINGLETON,
			EPS_EMULATOR_SERVER_SINGLETON,
			EPS_EMULATOR_HARDWARE_SINGLETON,
#endif
			NUM_SINGLETON_TYPES
		};
		
		class Factory
		{
		public:
			static Singleton * GetInstance(SingletonType type);
			//static Singleton * GetInstanceFromLocation(LocationIDType loc);
//#ifdef HOST
			static void Destroy(SingletonType type);
//#endif //HOST
			
		private:
			static bool Initialize(SingletonType type);
			
			static Singleton * instancePtrs[NUM_SINGLETON_TYPES];
			static bool isInitialized[NUM_SINGLETON_TYPES];
		};
	}
}



#endif /* FACTORY_H_ */
