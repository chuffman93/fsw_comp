/*! \file Dispatcher.cpp
 *  \brief Implementation File for the Dispatcher Class
 *
 *  This file contains the implementation of the Dispatcher class, which acts
 *  as the FSWPacket-routing entity in the Phoenix architecture.
 */

#include <utility>

#include "core/Dispatcher.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/WatchdogManager.h"

#include <iostream>
#include <signal.h>
#include <fcntl.h>
//#include "util/crc.h"
#include "core/CommandMessage.h"
#include "../src/HAL/Ethernet_Server.h"
#include "POSIX.h"

//#include "boards/backplane/dbg_led.h"

#ifndef WIN32
//extern "C"
//{
//#include "utils/debug/print_funcs.h"
//#include "util/delay.h"
//#include "drivers/cycle_counter.h"
//#include "drivers/gpio.h"

//#define PROFILING_PIN AVR32_PIN_PA25
//#define CLR_PROF_PIN() gpio_local_clr_gpio_pin(PROFILING_PIN);
//#define TGL_PROF_PIN() gpio_local_tgl_gpio_pin(PROFILING_PIN);
//}
//#else
//#define CLR_PROF_PIN()
#endif // WIN32

using namespace std;
//using namespace Phoenix::HAL;

#define DISPATCHER_DEBUG			1

#if DISPATCHER_DEBUG
#include <iostream>
#define DEBUG_PRINT(m, p) cout << m << "S: " << p->GetSource() \
        << ", D: " << p->GetDestination() \
        << ", N: " << p->GetNumber() \
        << ", R: " << (p->GetMessagePtr() ? p->GetMessagePtr()->IsResponse() : false) \
		<< ", T: " << ((int)p->GetMessagePtr()->GetType()) \
		<< ", O: " << ((int)p->GetMessagePtr()->GetOpcode()) \
        << ", P: " << p << endl;
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#define DEBUG_PRINT(m, p)
#endif // DEBUG



namespace Phoenix
{
    namespace Core
    {
        // Instantiate static members
    	char * Dispatcher::subsystemQueueName = "/subsystemQueueHandle";
    	char * Dispatcher::queueName = "/queueHandle";
    	size_t timer;

//
//    	char * Dispatcher::getQueueName()
//    	{
//    		return subsystemQueueName;
//    	}
//
//    	mqd_t * Dispatcher::getQueueHandle()
//    	{
//    		return &subsystemQueueHandle;
//    	}
//
//    	struct mq_attr * Dispatcher::getQueueAttr()
//    	{
//    		return &subsystemQueueAttr;
//    	}

void Dispatcher::Initialize(void)
{
	// Create the mutex and queue through the operating system.
//    		char * name = getQueueName();
//            subQinit = mqCreate(getQueueHandle(), getQueueAttr(), name);


/*
#ifndef WIN32
	// Register subsystem interrupts
	// TODO: Add other subsystems
	IntInit();
	IntInitPinInterrupt(INTERRUPT_PIN_PLD, &subsystemInterruptHandler, INTERRUPT_LEVEL_0, INTERRUPT_PULL_DOWN);
	IntInitPinInterrupt(INTERRUPT_PIN_ACS, &subsystemInterruptHandler, INTERRUPT_LEVEL_0, INTERRUPT_PULL_DOWN);
	IntInitPinInterrupt(INTERRUPT_PIN_EPS, &subsystemInterruptHandler, INTERRUPT_LEVEL_0, INTERRUPT_PULL_DOWN);
	IntInitPinInterrupt(INTERRUPT_PIN_COM, &subsystemInterruptHandler, INTERRUPT_LEVEL_0, INTERRUPT_PULL_DOWN);
	CREATE_TASK(DispatcherTask,
			(const signed char* const)"Dispatcher",
			1500,
			NULL,
			configMAX_PRIORITIES - 2,
			NULL);
#endif // WIN32
*/
}


bool Dispatcher::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized() && (subQinit == 0) && (qInit == 0));
}



void Dispatcher::DispatcherTask(void * params)
{
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	while (1)
	{
		uint64_t LastWakeTime = getTimeInMilis();

		DispatcherInterruptAlertType alert;

		//wdm->Kick();

		//TODO:ADD THREAD HANDLING and REMOVE RECIEVE COMPLETE STUFF and SIG_ACTION

		waitUntil(LastWakeTime, 1000);
	}
}



	// Adding the local register to the global registry map. This will require some kind of Mutex Protection so that multiple server threads don't mess up the Global
	// Registry Map - Umang
bool Dispatcher::AddRegistry(LocationIDType serverID,
		MessageHandlerRegistry * registry, Arbitrator * arby)
{
	pair<IteratorType, bool> ret;

	if (NULL == registry || NULL == arby)
	{
		return false;
	}

	// Get the semaphore.
	if (true == this->TakeLock(MAX_BLOCK_TIME))
	{
		// Insert the registry into the map with serverID as the key.
		DispatcherHandlerType * handlerStruct =
				new DispatcherHandlerType(registry, arby);
		ret = registryMap.insert(make_pair(serverID, handlerStruct));
		if (!ret.second)
		{
			delete handlerStruct;
		}
		this->GiveLock();
		return ret.second;
	}
	else
	{

		return false;
	}
}

bool Dispatcher::Dispatch(FSWPacket & packet)
{
   if (packet.GetDestination() >= HARDWARE_LOCATION_MIN && packet.GetDestination() < HARDWARE_LOCATION_MAX)
	{
	   DEBUG_COUT("Hardware Dispatch: YAY!");
		uint32_t ret;

		ret=DispatchToHardware(packet);
		return (0 == ret);
	}
	else
	{


		printf("Now sending the message to the queue\n");
		fflush(stdout);
		FSWPacket * tmpPacket;


		try
		{
			tmpPacket = new FSWPacket(packet);
		}
		catch (bad_alloc & e)
		{
			return false;
		}



		if (true == this->TakeLock(MAX_BLOCK_TIME))
		{
			printf("Dispatcher: Took lock\n");
			size_t numPackets = mq_size(queueHandle, queueAttr);

			if (numPackets < DISPATCHER_QUEUE_LENGTH)
			{
				printf("Dispatcher::Dispatch() Queue is not full\n");
				// Send the message via the message queue.
				bool ret = mq_timed_send(queueName, (&tmpPacket), MAX_BLOCK_TIME, 0);
				numPackets = mq_size(queueHandle, queueAttr);
				this->GiveLock();
				return ret;
			}
			else
			{
				printf("Dispatcher::Dispatch() Queue is full\n");
				delete tmpPacket;
				this->GiveLock();
				return false;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
}

DispatcherStatusEnum Dispatcher::WaitForDispatchResponse(
		const FSWPacket & packet, ReturnMessage & returnMessage)
{
	FSWPacket * ret;
	ReturnMessage * retMsg;
	size_t i;
	DEBUG_COUT("   Dispatcher: WaitForDispatchResponse() called");
	printf("Dispatcher: WaitForDispatchResponse() called\n");
	for (i = 0; i < DISPATCHER_MAX_RESPONSE_TRIES; ++i)
	{
		if (CheckQueueForMatchingPacket(packet, ret,
				&Dispatcher::IsPacketMatchingResponse))
		{
			printf("Dispatcher: WaitForDispatchResponse(): Matching FSWPacket found.\n");
			// Returned packet is a response to our command, so
			// return the result.
			DEBUG_COUT("   Dispatcher: WaitForDispatchResponse(): Matching FSWPacket found.");
			retMsg = dynamic_cast<ReturnMessage *> (ret->GetMessagePtr( ));
			returnMessage = (NULL == retMsg ? ReturnMessage( ) : *retMsg);
			delete ret;
			return DISPATCHER_STATUS_OK;
		}
		usleep(DISPATCHER_WAIT_TIME);

	}
	// At this point, see if the command we sent has been received at least.
	DEBUG_COUT("   Dispatch:  See if the packet we sent has been received.");
	//debug_led_set_led(6, LED_ON);

	if (CheckQueueForMatchingPacket(packet, ret,
			&Dispatcher::IsPacketSame))
	{
		// The command was not received, so it has been removed from the queue.
		delete ret;
		return DISPATCHER_STATUS_MSG_NOT_RCVD;
	}
	// The command was received, but no response has been placed in
	// the queue, so return that the operation failed.
	return DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT;
}


bool Dispatcher::Listen(LocationIDType serverID)
{
	FSWPacket * packet, tmpPacket;
	IteratorType it;

	DEBUG_COUT("Dispatcher: Listen() called with serverID: " << serverID);

	// Create a packet that can be compared with the incoming packets
	// to check if any are addressed to the desired server.

	tmpPacket.SetSource(serverID);

	if (!CheckQueueForMatchingPacket(tmpPacket, packet,
			&Dispatcher::IsPacketDestMatchingSource))
	{
		DEBUG_COUT("   Dispatcher: Listen(): No packets have been sent to this server.");
		return false;
	}
	DEBUG_COUT(" Dispatcher: Listen(): Found a packet, looking for a handler.");


	// A packet has been found, so try to find the handler in the
	// global registry list.- Umang
	if (true == this->TakeLock(MAX_BLOCK_TIME))
	{
		DEBUG_COUT("   Dispatcher: searching registry map for handler.");
		it = registryMap.find(serverID);
		this->GiveLock();
	}
	else
	{
		return false;
	}

	if (registryMap.end( ) == it)
	{
		DEBUG_COUT("   Dispatcher: Listen(): Didn't find a handler.");
		if (SendErrorResponse(ERROR_OPCODE_SERVER_NOT_REGISTERED,
				packet, VariableTypeData( )))
		{
		}
		return false;
	}
	// A handler exists, so check the permissions
	DEBUG_COUT("   Dispatcher: Listen(): Handler exists, checking permissions.");

	ArbitratorAuthStatusEnum arbyRet;
	if (ARBITRATOR_AUTH_STATUS_PERMISSION != (arbyRet
			= it->second->arby->Authenticate(*packet)))
	{
		DEBUG_COUT("   Dispatcher: Listen(): Don't have permissions, reject packet.");
		DEBUG_COUT("   Dispatcher: Listen(): Authenticate returned: " << arbyRet);
		if (SendErrorResponse(ERROR_OPCODE_PACKET_NOT_ALLOWED, packet,
				(uint32) arbyRet))
		{
		}
		return false;
	}
	// Permissions are correct, so invoke it and obtain the resulting message.
	DEBUG_COUT("   Dispatcher: Listen(): Permissions are correct, invoke the handler, and obtain the resulting message.");

	DispatcherTaskParameter parameters;
	parameters.registry = it->second->registry;
	cout<<"\t Registry: "<<parameters.registry<<endl;
	parameters.packet = packet;
	cout<<"\t packet: "<<parameters.packet<<endl;
	pthread_t TaskHandle;
	sem_init(&parameters.syncSem, SHARE_TO_THREADS, 1);
	sem_init(&parameters.doneSem, SHARE_TO_THREADS, 1);

	xSemaphoreTake(&parameters.syncSem, MAX_BLOCK_TIME, 0);
	pthread_attr_t TaskAttr;
	pthread_attr_init(&TaskAttr);
	struct sched_param schedParam;
	schedParam.__sched_priority = getpriority(PRIO_PROCESS, getpid())+1; //get priority of calling process and make this scheduler prio one lower
	pthread_attr_setschedparam(&TaskAttr,&schedParam);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_create(&TaskHandle,&TaskAttr, &InvokeHandler, &parameters);
	//printf("\r\nListening to the correct Access mode Dispatch Listen\r\n");

	xSemaphoreTake(&(parameters.syncSem), MAX_BLOCK_TIME, 0);
	usleep(1000);

	if (true
			!= xSemaphoreTake(&(parameters.doneSem), 1, 0))
	{
		if (SendErrorResponse(ERROR_OPCODE_HANDLER_TIMED_OUT, packet,
				VariableTypeData( )))
		{

		}
		DEBUG_COUT("HANDLER TIMED OUT");

		pthread_cancel(TaskHandle);
		sem_destroy(&(parameters.syncSem));
		sem_destroy(&(parameters.doneSem));
		return false;
	}
	pthread_join(TaskHandle, NULL);
	sem_destroy(&(parameters.syncSem));
	sem_destroy(&(parameters.doneSem));
	// Create a packet from the response.
	try
	{
		LocationIDType src = packet->GetSource( );
		packet->SetSource(packet->GetDestination( ));
		packet->SetDestination(src);
		packet->SetMessage(parameters.retMsg);

		delete parameters.retMsg;
	}
	catch (bad_alloc & e)
	{
		while (!mq_timed_send(queueName, &packet, MAX_BLOCK_TIME, 0));
		return false;
	}

	// Send the response back to the server that dispatched the
	// original message.
	while (!mq_timed_send(queueName, &packet, MAX_BLOCK_TIME, 0));
	return true;
}

bool Dispatcher::IsPacketMatchingResponse(const FSWPacket & packetIn,
		const FSWPacket & packetOut) const
{
	DEBUG_COUT("IsPacketMatchingRespone called");
	DEBUG_PRINT("Original FSWPacket - ", (&packetIn));
	DEBUG_PRINT("Queue FSWPacket    - ", (&packetOut));
	//debug_led_set_led(5, LED_ON);
	// Return true if *packetOut is a response to *packetIn.

	cout<<"Is response?: "<<packetOut.GetMessagePtr()->IsResponse()<<endl;
	cout<<"Matching location?: "<<(packetOut.GetDestination() == packetIn.GetSource())<<endl;
	cout<<"Same number?: "<<(packetOut.GetNumber() == packetIn.GetNumber())<<endl;

	return ((packetOut.GetMessagePtr( )->IsResponse( ))
			&& (packetOut.GetDestination( ) == packetIn.GetSource( ))
			&& (packetOut.GetNumber( ) == packetIn.GetNumber( )));
}

bool Dispatcher::IsPacketDestMatchingSource(const FSWPacket & packetIn,
		const FSWPacket & packetOut) const
{
	// Return true if *packetOut is being sent to the server in
	// packetIn->GetSource().
	DEBUG_COUT("IsPacketDestMatchingSource called");
	return ((!packetOut.GetMessagePtr( )->IsResponse( ))
			&& (packetOut.GetDestination( ) == packetIn.GetSource( )));
}

bool Dispatcher::IsPacketSame(const FSWPacket & packetIn,
		const FSWPacket & packetOut) const
{
	// Return true if packetIn and packetOut are equivalent.
	DEBUG_COUT("IsPacketSame called");
	return packetIn == packetOut;
}

bool Dispatcher::CheckQueueForMatchingPacket(const FSWPacket & packetIn,
		FSWPacket * &packetOut, PacketCheckFunctionType Check)
{
	size_t numPackets, i;
	FSWPacket * tmpPacket;

	// Get the semaphore
	if (true == this->TakeLock(MAX_BLOCK_TIME))
	{
		// Check for the first message in the queue

		if (mq_timed_receive(queueName, &packetOut, 0, DISPATCHER_MAX_DELAY) == false)
		{
			this->GiveLock();
			return false;
		}
		else
		{
			//debug_led_set_led(2, LED_ON);
			// There's at least one packet, so check if it matches packetIn.

			if(packetOut == NULL)
			{
				this->GiveLock();
				return false;
			}

			if ((this->*Check)(packetIn, *packetOut))
			{
				//debug_led_set_led(3, LED_ON);
				DEBUG_PRINT("Match 1 - ", packetOut);
				this->GiveLock();
				return true;
			}
			else
			{
				// Check the number of packets waiting in the queue.
				DEBUG_COUT("checking more packets");
				numPackets = mq_size(queueHandle, queueAttr);

				// Get each packet and check it against packetIn.
				for (i = 0; i < numPackets; ++i)
				{
					// Get the next packet.
					//mqd_t tmpqueueHandle = open(queueName, O_RDONLY);
					if (mq_timed_receive(queueName, &tmpPacket, 0, 0)
							== false)
					{
						//debug_led_set_led(4, LED_ON);
						// Error: There should have been a packet in
						// the queue, but there wasn't, so put
						// packetOut back.
						if (mq_timed_send(queueName, &packetOut, 1, 0)
								== false)
						{
							// Error: This is really bad because we
							// can't even preserve the state of
							// the queue.
							this->GiveLock();
							throw(0);
							break;
						}
						this->GiveLock();
						throw(1);
						break;
					}
					// Put packetOut back on the queue since it didn't
					// match packetIn.
					if (mq_timed_send(queueName, &packetOut, 1, 0)
							== false)
					{
						// Error
						this->GiveLock();
						throw(2);
						break;
					}
					packetOut = tmpPacket;
					DEBUG_PRINT("Loop - ", packetOut);

					// Check if packetOut matches packetIn
					if ((this->*Check)(packetIn, *packetOut))
					{
						//debug_led_set_led(6, LED_ON);
						// Found one!  Time to return!
						DEBUG_PRINT("Match 2 - ", packetOut);
						this->GiveLock();
						return true;
					}

				}
				// No packets were found, so put the last packet
				// back on the queue.
				if (mq_timed_send(queueName, &packetOut, 1, 0) == false)
				{
					// Error: This is really bad because we
					// can't even preserve the state of
					// the queue.
					this->GiveLock();
					throw(3);
				}
				this->GiveLock();
				return false;
			}
		}
	}
	else
	{
		DEBUG_PRINT("Sem Failed - ", packetOut);
		return false;
	}
}

bool Dispatcher::SendErrorResponse(ErrorOpcodeEnum errorCode,
		FSWPacket * packet, VariableTypeData data)
{
	ErrorMessage error(errorCode, data);
	ReturnMessage * ret;
	LocationIDType src;
	try
	{
		ret = new ReturnMessage(&error, false);
	}
	catch (bad_alloc & e)
	{

		while (!mq_timed_send(queueName, &packet, MAX_BLOCK_TIME, 0));
		return false;
	}
	src = packet->GetSource( );
	packet->SetSource(packet->GetDestination( ));
	packet->SetDestination(src);
	packet->SetMessage(ret);
	delete ret;
	while (!mq_timed_send(queueName, &packet, MAX_BLOCK_TIME, 0));
	return true;
}

void * Dispatcher::InvokeHandler(void * parameters)
{
	cout<<"\t Dispatcher: InvokeHandler(): invoke handler called"<<endl;
	DispatcherTaskParameter * parms =
			(DispatcherTaskParameter *) parameters;

	xSemaphoreTake(&(parms->doneSem), MAX_BLOCK_TIME, 0);
	sem_post(&(parms->syncSem));
	parms->retMsg = parms->registry->Invoke(*(parms->packet));
	cout<<"\t Dispatcher: InvokeHandler(): RetMsg Success: "<<parms->retMsg->GetSuccess()<<endl;
	cout<<"\t Dispatcher: InvokeHandler(): RetMsg Opcode:  "<<parms->retMsg->GetOpcode()<<endl;


	sem_post(&(parms->doneSem));

	pthread_exit(0);
}

//TODO:FIXME:Figure out hardware dispatching

uint32_t Dispatcher::DispatchToHardware( FSWPacket & packet)
{
	//todo: add semaphores for locking & real error values
	ssize_t packetLength;
	uint8_t * packetBuffer;
	ssize_t bytesCopied;
	size_t iterations;
	bool timedOut;

	/*
	if(transFlag == 1)
	{
		return -EBUSY;
	}
	else
	{
		transFlag = 1;
	}
	*/

	packetLength = packet.GetFlattenSize();
	printf("Hardware dispatch packet size %d",packet.GetFlattenSize());
	if(packetLength >= MAX_PACKET_SIZE)
	{
		//packet is too large
		return -1;
	}

	packetBuffer = (uint8_t *) malloc(packetLength);

	//check if whole packet was copied
	if(packet.Flatten(packetBuffer,packetLength) != packetLength)
	{
		//failed to flatten packet
		printf("failed at 2\n");
		return -2;
	}

	for(uint16_t i = 0; i < packetLength; i++)
	{
				printf("0x%02x ",packetBuffer[i]);
	}

	//write packet to /dev/phoenix_spi

	// Code for send the pheonix packet onto the SPI hardware
	//bytesCopied = write(spiFileDescriptor,packetBuffer,packetLength);


	// Get the instance for the Ethernet Server
	ETH_HALServer * cmd_server = dynamic_cast<ETH_HALServer *> (Factory::GetInstance(ETH_HALSERVER_SINGLETON));



	struct sockaddr_in fsin;    /* the from address of a client    */
	int alen= sizeof(fsin);
	int nbytes;

	memcpy(&fsin,cmd_server->ETH_GetSendFSin(packet.GetDestination()),sizeof(*(cmd_server->ETH_GetSendFSin(packet.GetDestination()))));
	alen= sizeof(fsin);
	// Code for sending the Pheonix Packet onto the UDP server
	nbytes= sendto(cmd_server->ETH_GetSendSocket(packet.GetDestination()), packetBuffer, packetLength,0,(struct sockaddr *) &fsin,alen);
	printf("\r\nDispatched to UDP Hardware %d bytes\r\n",nbytes);

	printf("\r\nDispatched to Hardware %d bytes to server number %d\r\n",nbytes,packet.GetDestination());
	printf("\r\n Dispatching to IP Address %s and port number %s",cmd_server->host[packet.GetDestination()],cmd_server->port_num_client[packet.GetDestination()]);


	/*
	 //fixme these are old SPI things
	printf("write called in dispatch to hardware\n");
	if(bytesCopied != packetLength)
	{
		//failed to copy packet into device file
		printf("failed to copy packet to device file :(\n");
		return -3;
	}
	sentPacket = false;

	uint64_t startTime = getTimeInMilis();

	uint64_t lastWakeTime = startTime;


	while(!sentPacket)
	{
		waitUntil(lastWakeTime, 500);
		if(timedOut)
		{
			//printf("Timeout YAYA!\n");
			spiReset();
			transFlag = 0;

			timedOut = false;
			return -1;
		}
		lastWakeTime = getTimeInMilis();
		if(200 ==(iterations++) ){ timedOut = true; }
	}

	//printf("send the packet?\n");
	sentPacket = false;
	transFlag = 0;
	*/

	return 0;
}


/*#ifndef WIN32
uint32_t Dispatcher::DispatchToHardware(HardwareLocationIDEnum loc, const FSWPacket & packet)
{
	SPIDeviceEnum dev;
	uint8_t chip;
	FSWPacket * packetOut;
	uint8 out;
	int i;
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	uint32_t ret;
	//debug_led_set_value(~0);

	if (!GetHardwareDeviceFromLocation(loc, dev, chip))
	{
	//	debug_led_set_led(7, LED_ON);
		return __LINE__;
	}

	SPIOpen(dev, chip);
	//debug_led_set_led(1, LED_ON);

	if (0 != SendPacketToHardware(dev, packet))
	{
	//	debug_led_set_led(7, LED_ON);
		SPIClose(dev);
		return __LINE__;
	}
	//debug_led_set_led(2, LED_ON);

	// Wait for the subsystem to finish processing the packet
	out = 0;
	i = 0;
	while (out != 1)
	{
		if (!SPIRead(dev, &out, 1))
		{
		//	debug_led_set_led(7, LED_ON);
			SPIClose(dev);
			return __LINE__;
		}
		if(i == MAX_DISPATCHER_LOOP)
		{
		//	debug_led_set_led(6, LED_ON);
			SPIClose(dev);
			return __LINE__;
		}
		cpu_delay_us(PROTOCOL_SPI_DELAY*100, CPU_PBACLK_FREQ_HZ);
		cpu_delay_us(PROTOCOL_SPI_DELAY*100, CPU_PBACLK_FREQ_HZ);
		i++;
	}
	//debug_led_set_led(3, LED_ON);

	packetOut = new FSWPacket();

	if (0 != (ret = dispatcher->GetPacketFromHardware(dev, *packetOut)))
	{
	//	debug_led_set_led(7, LED_ON);
		delete packetOut;
		SPIClose(dev);
		return __LINE__;
	}
	//debug_led_set_led(4, LED_ON);

	while (!xQueueSend(queueHandle, (void *)&packetOut, portMAX_DELAY));

	SPIClose(dev);
	//debug_led_set_led(0, LED_ON);

	return 0;
}*/

/* bool Dispatcher::GetHardwareDeviceFromLocation(HardwareLocationIDEnum loc, SPIDeviceEnum & dev, uint8_t & chip)
{

	switch (loc)
	{
	case HARDWARE_LOCATION_COM:
		dev = SUBSYSTEM_SPI;
		chip = SPI_CHIP_SUBSYSTEM3;
		break;
	case HARDWARE_LOCATION_EPS:
		dev = SUBSYSTEM_SPI;
		chip = SPI_CHIP_SUBSYSTEM4;
		break;
	case HARDWARE_LOCATION_ACS:
		dev = SUBSYSTEM_SPI;
		chip = SPI_CHIP_SUBSYSTEM2;
		break;
	case HARDWARE_LOCATION_PROP:
		dev = SUBSYSTEM_SPI;
		chip = SPI_CHIP_SUBSYSTEM1;
		break;
	case HARDWARE_LOCATION_PLD:
		dev = PAYLOAD_SPI;
		chip = SPI_CHIP_PAYLOAD;
		break;
	default:
		return false;
	}

	return true;
}*/

/*uint32_t Dispatcher::SendPacketToHardware(SPIDeviceEnum dev, const FSWPacket & packet)
{
	ReturnMessage retMsg;
	vector<uint8> messageBuffer;
	size_t numFrames;
	unsigned char out = 0;
	uint8 * buf;
	int i;

	if(packet.GetFlattenSize() >= MAX_NUMBER_FRAMES*MAX_FRAME_LENGTH)
	{
		//debug_led_set_led(7, LED_ON);
		return __LINE__;
	}

	buf = new uint8[packet.GetFlattenSize()];
	if (packet.GetFlattenSize() != packet.Flatten(buf, packet.GetFlattenSize()))
	{
		//debug_led_set_led(7, LED_ON);
		delete buf;
		return __LINE__;
	}

	if (packet.GetFlattenSize() == 0)
	{
		numFrames = 1;
	}
	else
	{
		numFrames = ((packet.GetFlattenSize()-1) / MAX_FRAME_LENGTH) + 1;
	}

	for (size_t frameNum = 0; frameNum < numFrames; ++frameNum)
	{
		uint8 pSize;
		if (frameNum == numFrames - 1)
		{
			pSize = packet.GetFlattenSize() % MAX_FRAME_LENGTH;
		}
		else
		{
			pSize = MAX_FRAME_LENGTH;
		}
		out = 0;
		i = 0;
		while (out != 1)
		{
			if(i == MAX_DISPATCHER_LOOP)
			{
				//debug_led_set_led(7, LED_ON);
				delete buf;
				return __LINE__;
			}
			if (frameNum == 0)
			{
				out = numFrames;
			}
			else
			{
				out = 0x40 | frameNum;
			}
			if (!SPIWrite(dev, &out, 1))
			{
				//debug_led_set_led(7, LED_ON);
				delete buf;
				return __LINE__;
			}

			cpu_delay_us(PROTOCOL_SPI_DELAY*100, CPU_PBACLK_FREQ_HZ);
			cpu_delay_us(PROTOCOL_SPI_DELAY*100, CPU_PBACLK_FREQ_HZ);
			if (!SPIRead(dev, &out, 1))
			{
				//debug_led_set_led(7, LED_ON);
				delete buf;
				return __LINE__;
			}

			cpu_delay_us(PROTOCOL_SPI_DELAY*100, CPU_PBACLK_FREQ_HZ);
			cpu_delay_us(PROTOCOL_SPI_DELAY*100, CPU_PBACLK_FREQ_HZ);
			i++;
		}
		//debug_led_set_led(0, LED_ON);

		// Send packet size
		if (!SPIWrite(dev, &pSize, 1))
		{
			//debug_led_set_led(7, LED_ON);
			delete buf;
			return __LINE__;
		}

		cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);
		cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);

		for (size_t i = 0; i < pSize; ++i)
		{
			if (!SPIWrite(dev, &buf[MAX_FRAME_LENGTH*frameNum+i], 1))
			{
				//debug_led_set_led(7, LED_ON);
				delete buf;
				return __LINE__;
			}

			cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);
			cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);
		}
	}
	delete buf;

	return 0;
}*/

/* uint32_t Dispatcher::GetPacketFromHardware(SPIDeviceEnum dev, FSWPacket & packet)
{
	vector<uint8> messageBuffer;
	crc_t crc;
	size_t numPackets;
	unsigned char out = 0;
	unsigned char length;
	uint8 packetNum;
	crc_t recCrc;

	numPackets = 1;

	packetNum = 0;
	crc = INITIAL_REMAINDER;

	while (packetNum != numPackets)
	{
		if (!SPIRead(dev, &out, 1))
		{
			debug_led_set_led(7, LED_ON);
			return __LINE__;
		}
		//debug_led_set_value(~out);
		//debug_led_set_value(~0);
		if ((out & 0x40) == 0)
		{
			numPackets = out & 0x3f;
			packetNum = 0;
		}
		else
		{
			if (packetNum != (out & 0x3f))
			{
				debug_led_set_led(7, LED_ON);
				return __LINE__;
			}
		}

		cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);
		cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);

		if (!SPIRead(dev, &length, 1))
		{
			debug_led_set_led(7, LED_ON);
			return __LINE__;
		}

		cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);
		cpu_delay_265us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);

		if (length < PACKET_OVERHEAD_SIZE)
		{

			debug_led_set_value(~length);
			debug_led_set_value(~0);
			debug_led_set_led(7, LED_ON);
			return __LINE__;
		}


		for (size_t i = 0; i < length; ++i)
		{
			size_t comp;
			if (!SPIRead(dev, &out, 1))
			{
				debug_led_set_led(7, LED_ON);
				return __LINE__;
			}

			cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);
			cpu_delay_us(PROTOCOL_SPI_DELAY, CPU_PBACLK_FREQ_HZ);

			uint8 temp = out & 0xff;

			if (packetNum != 0)
			{
				comp = i + 100;
			}
			else
			{
				comp = i;
			}

			switch (comp)
			{
			case 0:
				packet.SetSource((out << 8) & 0xff00);
				break;
			case 1:
				packet.SetSource(packet.GetSource() | (out & 0xff));
				break;
			case 2:
				packet.SetDestination((out << 8) & 0xff00);
				break;
			case 3:
				packet.SetDestination(packet.GetDestination() | (out & 0xff));
				break;
			case 4:
				packet.SetNumber((out << 8) & 0xff00);
				break;
			case 5:
				packet.SetNumber(packet.GetNumber() | (out & 0xff));
				break;
			case 6:
				packet.SetTimestamp(((uint32)out << 24) & 0xff000000);
				break;
			case 7:
				packet.SetTimestamp(packet.GetTimestamp() | (((uint32)out << 16) & 0x00ff0000));
				break;
			case 8:
				packet.SetTimestamp(packet.GetTimestamp() | (((uint32)out << 8)  & 0x0000ff00));
				break;
			case 9:
				packet.SetTimestamp(packet.GetTimestamp() | (out & 0xff));
				messageBuffer.clear();
				break;
			default:
				messageBuffer.push_back(out);
				break;
			}

			if ((packetNum < (numPackets - 1)) || ((packetNum == (numPackets - 1)) && i < length - 2))
			{
				crc = crcFast(&temp, 1, crc);
			}
		}
		++packetNum;
	}
	//debug_led_set_led(3, LED_ON);
	recCrc = messageBuffer.back() & 0xff;
	messageBuffer.pop_back();
	recCrc |= (messageBuffer.back() << 8) & 0xff00;
	messageBuffer.pop_back();

	if (recCrc != crc)
	{
		debug_led_set_led(7, LED_ON);
		return __LINE__;
	}

	packet.SetMessage(NULL);
	Message * msg = Message::CreateMessage(messageBuffer.data(), messageBuffer.size());
	packet.SetMessage(msg);
	delete msg;

	return 0;
}
#endif // WIN32*/

Dispatcher::Dispatcher(void)
		: Singleton(), registryMap()
{

	mq_unlink(subsystemQueueName);
	mq_unlink(queueName);
	subQinit = mqCreate(&subsystemQueueHandle, &subsystemQueueAttr, subsystemQueueName);

	qInit = mqCreate(&queueHandle, &queueAttr, queueName);


}

Dispatcher::Dispatcher(const Dispatcher & source)
{
	// Left Intentionally Empty
}

#ifdef TEST
void Dispatcher::Destroy(void)
{
	mq_unlink(subsystemQueueName);
	mq_unlink(queueName);
}
#endif

Dispatcher::~Dispatcher(void)
{
	while (!registryMap.empty( ))
	{
		IteratorType it = registryMap.begin( );
		delete it->second;
		registryMap.erase(it);
	}
	registryMap.clear( );
	mq_unlink(subsystemQueueName);
	mq_unlink(queueName);
}

Dispatcher & Dispatcher::operator=(const Dispatcher & source)
{
	return *this;
}

    }
}


