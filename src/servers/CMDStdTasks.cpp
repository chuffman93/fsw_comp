/*
 * CMDStdTasks.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#include <list>

#include "servers/SCHServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"

#include "util/FileHandler.h"
#include <cstdio>

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;
using namespace std;

namespace Phoenix
{
	namespace Servers
	{
		void CMDBufferParse(char * readBuffer, size_t bufferSize)
		{
			if(readBuffer == NULL)
			{
				return;
			}
			////WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			//minimum size of a packet
			
			packetLoop:
			while(6 < bufferSize)
			{
				//wdm->Kick();
				FSWPacket * packet = new FSWPacket((uint8 *) readBuffer, bufferSize);
				readBuffer += packet->GetFlattenSize();
				bufferSize -= packet->GetFlattenSize();
				LocationIDType packetDest = packet->GetDestination();
				ServerLocationIDEnum destination;
				
				if(packetDest >= SERVER_LOCATION_MIN && packetDest < SERVER_LOCATION_MAX)
				{
					destination = static_cast<ServerLocationIDEnum>(packetDest);
				}
				
				if(destination == SERVER_LOCATION_CMD)
				{
					FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
					uint32 enumArray[1] = {VAR_TYPE_ENUM_STRING};
					void * outputArray[1] = {NULL};
					if(ExtractParameters(*packet, enumArray, 1, outputArray))
					{
						continue;
					}
						
					string * fileName = (string *) outputArray[0];
					const char * file = fileName->c_str();
					fileHandler->DeleteFile(file);
					return;
					
				}
				
				//uint8 opcode = packet->GetMessagePtr()->GetOpcode();

				Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

				//Dispatch packet, if it fails return DISPATCH_FAILED

				printf("\r\nCMDParse:: Dispatching the packet to destination %d\r\n",packet->GetDestination());
				printf("\r\nCMDParse:: Dispatching the packet to source %d\r\n",packet->GetSource());

				if(!dispatcher->Dispatch(*packet))
				{
					delete packet;
					continue;
				}

				ReturnMessage retMsg;
				DispatcherStatusEnum stat;
				//Wait for return message, if it fails return status response from dispatcher
				if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
				{
					delete packet;
					continue;
				}
				delete packet;
				ReturnMessage * ret = new ReturnMessage(retMsg);
				//process response
				MessageProcess(packetDest, ret);
				usleep(500000);
			}
			return;
		}
	}
}
