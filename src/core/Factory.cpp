/*
* Factory.cpp
*
* Created: 8/22/2013 11:28:28 AM
*  Author: Admin
*/

#include "servers/ACSServer.h"
#include "servers/CDHServer.h"
#include "servers/CMDServer.h"
#include "servers/COMServer.h"
#include "servers/EPSServer.h"
#include "servers/ERRServer.h"
#include "servers/FMGServer.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/SCHServer.h"
#include "servers/CDHServer.h"
#include "core/Factory.h"
#include "core/Singleton.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
#include "HAL/SPI_Server.h"
#include "util/Logger.h"
#include <stdio.h>
#include <stdlib.h>

using namespace AllStar::Servers;

namespace AllStar {
namespace Core {

Singleton * Factory::instancePtrs[NUM_SINGLETON_TYPES] = {NULL};
bool Factory::isInitialized[NUM_SINGLETON_TYPES] = {false};

bool Factory::Initialize(SingletonType type) {
	Singleton * instance;
	if (!isInitialized[type]) {
		switch (type) {
		case DISPATCHER_SINGLETON:
			instance = new Dispatcher( );
			break;
		case MODE_MANAGER_SINGLETON:
			instance = new ModeManager( );
			break;
		case WATCHDOG_MANAGER_SINGLETON:
			instance = new WatchdogManager( );
			break;
		case LOGGER_SINGLETON:
			instance = new Logger( );
			break;
		case ACS_SERVER_SINGLETON:
			instance = new ACSServer("ACS", SERVER_LOCATION_ACS);
			break;
		case CMD_SERVER_SINGLETON:
			instance = new CMDServer("CMD", SERVER_LOCATION_CMD);
			break;
		case COM_SERVER_SINGLETON:
			instance = new COMServer("COM", SERVER_LOCATION_COM);
			break;
		case EPS_SERVER_SINGLETON:
			instance = new EPSServer("EPS", SERVER_LOCATION_EPS);
			break;
		case FMG_SERVER_SINGLETON:
			instance = new FMGServer("FMG", SERVER_LOCATION_FMG);
			break;
		case GPS_SERVER_SINGLETON:
			instance = new GPSServer("GPS", SERVER_LOCATION_GPS);
			break;
		case PLD_SERVER_SINGLETON:
			instance = new PLDServer("PLD", SERVER_LOCATION_PLD);
			break;
		case SCH_SERVER_SINGLETON:
			instance = new SCHServer("SCH", SERVER_LOCATION_SCH);
			break;
		case CDH_SERVER_SINGLETON:
			instance = new CDHServer("CDH", SERVER_LOCATION_CDH);
			break;
		case ERR_SERVER_SINGLETON:
			instance = new ERRServer("ERR", SERVER_LOCATION_ERR);
			break;
		case SPI_HALSERVER_SINGLETON:
			instance = new SPI_HALServer();
			break;
		default:
			cout<<"Factory Error!: Unknown singleton!"<<endl;
			instance = NULL;
			return false;
		}

		// The order of these checks is important, as we need to verify that instance is not NULL.
		isInitialized[type] = ((instance != NULL)  && (instance->IsFullyInitialized()));
		if (isInitialized[type]) {
			instancePtrs[type] = instance;
		} else {
			//Destroy(type);
			delete instance;
		}
	}

	return isInitialized[type];
}



Singleton * Factory::GetInstance(SingletonType type) {
	while(!isInitialized[type]) {
		Initialize(type);
	}

	return instancePtrs[type];
}

}
}
