/*
* CMDServer.cpp
*
*  Created on: Feb 12, 2013
*      Author: Caitlyn
*  Modified by:
*/

#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ComMode.h"
#include "core/SystemMode.h"
#include "POSIX.h"

#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		
		CMDServer::CMDServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			// Left Intentionally Blank
		}

		CMDServer::~CMDServer()
		{
			// Left Intentionally Blank
		}
		
		void CMDServer::Initialize(void)
		{
			
		}
		
#ifdef HOST
		void CMDServer::Destroy(void)
		{
			
		}
#endif
		
		bool CMDServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		CMDServer & CMDServer::operator=(const CMDServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}

		void CMDServer::SubsystemLoop(void)
		{
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			
			const SystemMode * mode;  

			uint8 * readBuffer;
			size_t readSize = 0;

			CMDFiles[0] = (char *)"CMD_List_1.dat";	// ACS or ALL
			CMDFiles[1] = (char *)"CMD_List_2.dat"; // COM
			CMDFiles[2] = (char *)"CMD_List_3.dat"; // EPS
			CMDFiles[3] = (char *)"CMD_List_4.dat"; // GPS
			CMDFiles[4] = (char *)"CMD_List_5.dat"; // PLD
			
			//VariableTypeData str_hold("test_schedule.dat");
			//ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_CMD, SERVER_LOCATION_SCH, 0, 1, MESSAGE_TYPE_COMMAND, SCH_BUILD_SCHEDULE_CMD, str_hold);			
			
			while(1)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();
				//debug_led_set_led(2, LED_TOGGLE);
				//check if there is a list of commands to execute
				for(int i = 0; i < 5; i++)
				{
					//check if file exists
					if((fileHandler->FileExists(CMDFiles[i])) >= 0)
					{
						//read the file into a buffer
						readBuffer = fileHandler->ReadFile(CMDFiles[i], &readSize);
						//try to create packets from the buffer and send them
						CMDBufferParse(((char *) readBuffer), readSize);
						//fileHandler->DeleteFile(CMDFiles[i]);
					}usleep(1000);
				}
				
				/*
				// For deleting files, shouldn't be executed in COM mode.
				// Check current mode
				mode = modeManager->GetMode();
				if(!(mode == ComMode::GetInstance()))
				{
					//read the file into a buffer
					readBuffer = fileHandler->ReadFile(CMDFiles[4], &readSize);
					//try to create packets from the buffer and send them
					
					CMDBufferParse(((char *) readBuffer), readSize);
					//fileHandler->DeleteFile(CMDFiles[4]);
				}
				*/
				
				//TODO: what is this? usleep is in microseconds btw
				//each iteration is 30 seconds
				for(int i = 0; i < 30; i++)
				{
					//wdm->Kick();
					usleep(1000);
				}
				
			}
		}


		void CMDServer::Update(const SystemMode * mode)
		{
			// Called by mode manager each time mode changes
			//* Ex: Needs to do things to close mode 1, enter mode 2
			//* Setup and tear down between modes



		}


		bool CMDServer::RegisterHandlers()
		{
			bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}


	} // End Namespace servers
} // End Namespace Phoenix
