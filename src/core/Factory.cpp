/*
* Factory.cpp
*
* Created: 8/22/2013 11:28:28 AM
*  Author: Admin
*/

#include "servers/ACSServer.h"
#include "servers/CMDServer.h"
#include "servers/COMServer.h"
#include "servers/EPSServer.h"
#include "servers/ErrorOctopus.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/SCHServer.h"
#include "servers/THMServer.h"
#include "servers/ErrorQueue.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include "RFInterface.h"
#include "COMMockHardware.h"
#include "EPSEmulatorServer.h"
#include "EPSMockHardware.h"
#endif

#include "core/Factory.h"
#include "core/Singleton.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"

#include "util/FileHandler.h"

using namespace Phoenix::Servers;

#define FACTORY_DEBUG			1

#if FACTORY_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_FACTORY_COUT(m) std::cout << m << std::endl;
#else
#define DEBUG_FACTORY_COUT(m)
#endif // DEBUG

namespace Phoenix
{
	namespace Core
	{
		Singleton * Factory::instancePtrs[NUM_SINGLETON_TYPES] = {NULL};
		bool Factory::isInitialized[NUM_SINGLETON_TYPES] = {false};
		
		bool Factory::Initialize(SingletonType type)
		{
			Singleton * instance;
			if (!isInitialized[type])
			{
				switch (type)
				{
					case DISPATCHER_SINGLETON:
						Dispatcher::Initialize( );
						instance = new Dispatcher( );
						break;
					case MODE_MANAGER_SINGLETON:
						ModeManager::Initialize( );
						instance = new ModeManager( );
						break;
					case WATCHDOG_MANAGER_SINGLETON:
						WatchdogManager::Initialize( );
						instance = new WatchdogManager( );
						break;
					case ERROR_QUEUE_SINGLETON:
						ErrorQueue::Initialize( );
						instance = new ErrorQueue( );
						break;
					case FILE_HANDLER_SINGLETON:
						//FileHandler::Initialize( );
						//instance = new FileHandler( );
						break;
					case ACS_SERVER_SINGLETON:
						ACSServer::Initialize( );
						instance = new ACSServer("ACS Server", SERVER_LOCATION_ACS);
						break;
					case CMD_SERVER_SINGLETON:
						CMDServer::Initialize( );
						instance = new CMDServer("CMD Server", SERVER_LOCATION_CMD);
						break;
					case COM_SERVER_SINGLETON:
						COMServer::Initialize( );
						instance = new COMServer("COM Server", SERVER_LOCATION_COM);
						break;
					case EPS_SERVER_SINGLETON:
						EPSServer::Initialize( );
						instance = new EPSServer("EPS Server", SERVER_LOCATION_EPS);
						break;
					case ERR_SERVER_SINGLETON:
						ErrorOctopus::Initialize( );
						instance = new ErrorOctopus("Error Octopus", SERVER_LOCATION_ERR);
						break;
					case GPS_SERVER_SINGLETON:
						GPSServer::Initialize( );
						instance = new GPSServer("GPS Server", SERVER_LOCATION_GPS);
						break;
					case PLD_SERVER_SINGLETON:
						PLDServer::Initialize( );
						instance = new PLDServer("PLD Server", SERVER_LOCATION_PLD);
						break;
 					case SCH_SERVER_SINGLETON:
 						SCHServer::Initialize( );
 						instance = new SCHServer("SCH Server", SERVER_LOCATION_SCH);
 						break;
					case THM_SERVER_SINGLETON:
						THMServer::Initialize( );
						instance = new THMServer("THM Server", SERVER_LOCATION_THM);
						break;
#ifdef NOT
					case RF_INTERFACE_SINGLETON:
						Phoenix::Emulators::RFInterface::Initialize( );
						instance = new Phoenix::Emulators::RFInterface();
						break;
					case GROUND_COM_SERVER_SINGLETON:
						COMServer::Initialize( );
						instance = new COMServer("Ground COM Server", SERVER_LOCATION_GND_COM);
						break;
					case FLIGHT_COM_MOCK_HARDWARE_SINGLETON:
						Phoenix::Emulators::COMMockHardware::Initialize( );
						instance = new Phoenix::Emulators::COMMockHardware("Flight COM Mock Emulator", HARDWARE_LOCATION_COM);
						break;
					case GROUND_COM_MOCK_HARDWARE_SINGLETON:
						Phoenix::Emulators::COMMockHardware::Initialize( );
						instance = new Phoenix::Emulators::COMMockHardware("Ground COM Mock Emulator", HARDWARE_LOCATION_GND_COM);
						break;
					case EPS_EMULATOR_SERVER_SINGLETON:
						Phoenix::Emulators::EPSEmulatorServer::Initialize( );
						instance = new Phoenix::Emulators::EPSEmulatorServer("EPS Server Emulator", SERVER_LOCATION_EPS);
						break;
					case EPS_EMULATOR_HARDWARE_SINGLETON:
						Phoenix::Emulators::EPSMockHardware::Initialize( );
						instance = new Phoenix::Emulators::EPSMockHardware("EPS Mock Emulator", HARDWARE_LOCATION_EPS);
						break;
#endif
					default:
						instance = NULL;
						return false;
					
				}
				// The order of these checks is important, as we need to verify that instance is not NULL.
				isInitialized[type] = ((instance != NULL)  && (instance->IsFullyInitialized()));
				if (isInitialized[type])
				{
					instancePtrs[type] = instance;
				}
				else
				{
					//Destroy(type);
					delete instance;
				}
			}
			
			return isInitialized[type];
		}
		
		
		
		Singleton * Factory::GetInstance(SingletonType type)
		{
			while(!isInitialized[type])
			{
				Initialize(type);
			}
			return instancePtrs[type];
		}
		
		/*Singleton * Factory::GetInstanceFromLocation(LocationIDType loc)
		{
			Server * server;
			Singleton * obj;
			
			for (SingletonType type = SINGLETON_TYPE_START; type < NUM_SINGLETON_TYPES; ++type)
			{
				obj = GetInstance(type);
				if ((server = dynamic_cast<Server *>(obj)) != NULL)
				{
					if (server->GetID() == loc)
					{
						return obj;
					}
				}
			}
		} */
		
#ifdef TEST
	 void Factory::Destroy(SingletonType type)
		{
			switch (type)
			{
				case DISPATCHER_SINGLETON:
					Dispatcher::Destroy( );
					break;
				case MODE_MANAGER_SINGLETON:
					ModeManager::Destroy( );
					break;
//				case WATCHDOG_MANAGER_SINGLETON:
//					WatchdogManager::Destroy( );
//					break;
				case ERROR_QUEUE_SINGLETON:
					ErrorQueue::Destroy( );
					break;
//				case FILE_HANDLER_SINGLETON:
//					FileHandler::Destroy( );
//					break;
				case ACS_SERVER_SINGLETON:
					ACSServer::Destroy( );
					break;
				case CMD_SERVER_SINGLETON:
					CMDServer::Destroy( );
					break;
				case COM_SERVER_SINGLETON:
					COMServer::Destroy( );
					break;
				case EPS_SERVER_SINGLETON:
					EPSServer::Destroy( );
					break;
				case ERR_SERVER_SINGLETON:
					ErrorOctopus::Destroy( );
					break;
				case GPS_SERVER_SINGLETON:
					GPSServer::Destroy( );
					break;
				case PLD_SERVER_SINGLETON:
					PLDServer::Destroy( );
					break;
 				case SCH_SERVER_SINGLETON:
 					SCHServer::Destroy( );
 					break;
				case THM_SERVER_SINGLETON:
					THMServer::Destroy( );
					break;
#ifdef NOT
				case RF_INTERFACE_SINGLETON:
					Phoenix::Emulators::RFInterface::Destroy( );
					break;
				case GROUND_COM_SERVER_SINGLETON:
					//COMServer::Destroy( );
					break;
				case FLIGHT_COM_MOCK_HARDWARE_SINGLETON:
					Phoenix::Emulators::COMMockHardware::Destroy( );
					break;
				case GROUND_COM_MOCK_HARDWARE_SINGLETON:
					//Phoenix::Emulators::COMMockHardware::Destroy( );
					break;
				case EPS_EMULATOR_SERVER_SINGLETON:
					Phoenix::Emulators::EPSEmulatorServer::Destroy( );
					break;
				case EPS_EMULATOR_HARDWARE_SINGLETON:
					Phoenix::Emulators::EPSMockHardware::Destroy( );
					break;
#endif
				default:
					return;
			}
			
			if(instancePtrs[type] != NULL)
			{
				printf("deleting things\n");
				delete instancePtrs[type];
				instancePtrs[type] = NULL;
				isInitialized[type] = false;
			}
		}
#endif // TEST
	}
}
