/*
* CMDServer.cpp
*
*  Created on: Feb 12, 2013
*      Author: Caitlyn
*  Modified by:
*/

#include "servers/CMDHandlers.h"
#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "POSIX.h"

#include <iostream>

#include "core/WatchdogManager.h"
#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		//Message Handler for ACP Protocol Switches
		static CMDSwitchProtocolHandler * cmdSwitchProtocolHandler;
		int CMDServer::subsystem_acp_protocol[HARDWARE_LOCATION_MAX];
		
		CMDServer::CMDServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			for(int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++){
				subsystem_acp_protocol[i] = ACP_PROTOCOL_SPI;
			}
		}

		CMDServer::~CMDServer()
		{
			// Left Intentionally Blank
		}
		
		void CMDServer::Initialize(void)
		{
			cmdSwitchProtocolHandler = new CMDSwitchProtocolHandler();
		}
		
#ifdef TEST
		void CMDServer::Destroy(void)
		{
			delete cmdSwitchProtocolHandler;
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

		void CMDServer::Update(SystemModeEnum mode)
		{
			// Called by mode manager each time mode changes
			//* Ex: Needs to do things to close mode 1, enter mode 2
			//* Setup and tear down between modes



		}

		bool CMDServer::RegisterHandlers()
		{
			bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			// CMD Command Opcode
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH), cmdSwitchProtocolHandler);
			success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH), true);

			/*
			for(int opcode = EPS_SUB_ERROR_MIN; opcode < EPS_SUB_ERROR_MAX; opcode++)
			{
				success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), epsErrorHandler);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}
			*/


			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void CMDServer::SubsystemLoop(void)
		{
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			SystemModeEnum mode;

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
				while(Listen(id));
				uint64_t LastTimeTick = getTimeInMillis();
				//wdm->Kick();
				//debug_led_set_led(2, LED_TOGGLE);
				//check if there is a list of commands to execute
//				for(int i = 0; i < 5; i++)
//				{
//					//check if file exists
//					//if((fileHandler->FileExists(CMDFiles[i])) >= 0)
//					//{
//						//read the file into a buffer
//						readBuffer = fileHandler->ReadFile(CMDFiles[i], &readSize);
//
////						if (i==0)
////						{
////							printf("\r\nPrinting the buffer read\r\n");
////							// Check if the Read File worked it works- Umang
////							for (int pq=0;pq<readSize;pq++)
////								printf("\t %02x",readBuffer[pq]);
////						}
//
//
//
//						//try to create packets from the buffer and send them
//						CMDBufferParse(((char *) readBuffer), readSize);
//						//fileHandler->DeleteFile(CMDFiles[i]);
//					//}
//					usleep(1000);
//				}

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

				waitUntil(LastTimeTick, 1000);

			}
		}
	} // End Namespace servers
} // End Namespace Phoenix
