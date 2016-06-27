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
#include "util/Logger.h"

#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <string>
#include "core/CommandMessage.h"
#include "../src/HAL/Ethernet_Server.h"
#include "HAL/SPI_Server.h"
#include "servers/CMDServer.h"
#include "POSIX.h"

using namespace std;

#define DISPATCHER_DEBUG			1

#if DISPATCHER_DEBUG
#include <iostream>
#define DEBUG_PRINT(m, p) cout << m << "S: " << p->GetSource() \
        << ", D: " << p->GetDestination() \
        << ", N: " << p->GetNumber() \
        << ", R: " << (p->IsResponse()) \
		<< ", T: " << ((int)p->GetType()) \
		<< ", O: " << ((int)p->GetOpcode()) \
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
char * Dispatcher::queueNameRX = (char *) "/queueHandleRX";
size_t timer;

void Dispatcher::Initialize(void)
{
	// Intentionally left empty
}

bool Dispatcher::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized() && (qInitRX == 0));
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	if(packet.GetDestination() < HARDWARE_LOCATION_MIN || packet.GetDestination() > SERVER_LOCATION_MAX){
		return false;
	}
   if (packet.GetDestination() >= HARDWARE_LOCATION_MIN && packet.GetDestination() < HARDWARE_LOCATION_MAX)
	{
	   DEBUG_COUT("Hardware Dispatch: YAY!");
		uint32_t ret;

		ret=DispatchToHardware(packet);
		return (0 == ret);
	}
	else
	{


		logger->Log("Now sending the message to the queue", LOGGER_LEVEL_DEBUG);


		if (true == this->TakeLock(MAX_BLOCK_TIME))
		{
			logger->Log("Dispatcher: Took lock", LOGGER_LEVEL_DEBUG);
			size_t numPackets = mq_size(queueHandleRX, queueAttrRX);
			FSWPacket * tmpPacket;
			try
			{
				tmpPacket = new FSWPacket(packet);
			}
			catch (bad_alloc & e)
			{
				return false;
			}
			if (numPackets < DISPATCHER_QUEUE_LENGTH)
			{
				logger->Log("Dispatcher::Dispatch() Queue is not full", LOGGER_LEVEL_DEBUG);
				// Send the message via the message queue.
				bool ret = mq_timed_send(queueNameRX, (&tmpPacket), MAX_BLOCK_TIME, 0);
				numPackets = mq_size(queueHandleRX, queueAttrRX);
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

DispatcherStatusEnum Dispatcher::WaitForDispatchResponse(const FSWPacket & packet, FSWPacket ** retPacketin)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	//ReturnMessage * retMsg;
	size_t i;
	FSWPacket * retPacket;
	logger->Log("   Dispatcher: WaitForDispatchResponse() called", LOGGER_LEVEL_DEBUG);
	for (i = 0; i < DISPATCHER_MAX_RESPONSE_TRIES; ++i)
	{
		if (CheckQueueForMatchingPacket(packet, retPacket,
				&Dispatcher::IsPacketMatchingResponse))
		{
			logger->Log("    Dispatcher: WaitForDispatchResponse(): Matching FSWPacket found.", LOGGER_LEVEL_DEBUG);

			*retPacketin = retPacket;
			return DISPATCHER_STATUS_OK;
		}
		usleep(DISPATCHER_WAIT_TIME);
	}

	// At this point, see if the command we sent has been received at least.
	logger->Log("   Dispatch:  See if the packet we sent has been received.", LOGGER_LEVEL_DEBUG);
	//debug_led_set_led(6, LED_ON);

	if (CheckQueueForMatchingPacket(packet, retPacket,
			&Dispatcher::IsPacketSame))
	{
		logger->Log("   Dispatch: Command not received, removed from queue", LOGGER_LEVEL_ERROR);
		return DISPATCHER_STATUS_MSG_NOT_RCVD;
	}

	// The command was received, but no response has been placed in
	// the queue, so return that the operation failed.
	logger->Log("   Dispatch: Command received, but no response sent", LOGGER_LEVEL_ERROR);
	return DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT;
}

bool Dispatcher::Listen(LocationIDType serverID)
{
	FSWPacket * packet;
	FSWPacket tmpPacket;

	IteratorType it;

	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("Dispatcher: Listen() called with serverID: %u", serverID, LOGGER_LEVEL_DEBUG);

	// Create a packet that can be compared with the incoming packets
	// to check if any are addressed to the desired server.

	tmpPacket.SetSource(serverID);

	if (!CheckQueueForMatchingPacket(tmpPacket, packet,
			&Dispatcher::IsPacketDestMatchingSource))
	{
		logger->Log("   Dispatcher: Listen(): No packets have been sent to this server.", LOGGER_LEVEL_DEBUG);
		return false;
	}
	logger->Log(" Dispatcher: Listen(): Found a packet, looking for a handler.", LOGGER_LEVEL_DEBUG);

	// A packet has been found, so try to find the handler in the
	// global registry list.- Umang
	if (true == this->TakeLock(MAX_BLOCK_TIME))
	{
		logger->Log("   Dispatcher: searching registry map for handler.", LOGGER_LEVEL_DEBUG);
		it = registryMap.find(serverID);
		this->GiveLock();
	}
	else
	{
		delete packet;
		return false;
	}

	if (registryMap.end( ) == it)
	{
		logger->Log("   Dispatcher: Listen(): Didn't find a handler.", LOGGER_LEVEL_DEBUG);
		if (SendErrorResponse(ERROR_OPCODE_SERVER_NOT_REGISTERED,
				packet, VariableTypeData( )))
		{
		}
		delete packet;
		return false;
	}
	// A handler exists, so check the permissions
	logger->Log("   Dispatcher: Listen(): Handler exists, checking permissions.", LOGGER_LEVEL_DEBUG);

	ArbitratorAuthStatusEnum arbyRet;
	if (ARBITRATOR_AUTH_STATUS_PERMISSION != (arbyRet
			= it->second->arby->Authenticate(*packet)))
	{
		logger->Log("   Dispatcher: Listen(): Don't have permissions, reject packet.", LOGGER_LEVEL_ERROR);
		logger->Log("   Dispatcher: Listen(): Authenticate returned: %d", arbyRet, LOGGER_LEVEL_ERROR);
		if (SendErrorResponse(ERROR_OPCODE_PACKET_NOT_ALLOWED, packet,
				(uint32) arbyRet))
		{
		}
		delete packet;
		return false;
	}
	// Permissions are correct, so invoke it and obtain the resulting message.
	logger->Log("   Dispatcher: Listen(): Permissions are correct, invoke the handler, and obtain the resulting message.", LOGGER_LEVEL_DEBUG);

	DispatcherTaskParameter parameters;
	parameters.registry = it->second->registry;
	parameters.packet = packet;
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
		logger->Log("HANDLER TIMED OUT", LOGGER_LEVEL_ERROR);

		pthread_cancel(TaskHandle);
		sem_destroy(&(parameters.syncSem));
		sem_destroy(&(parameters.doneSem));
		delete packet;
		return false;
	}
	pthread_join(TaskHandle, NULL);
	sem_destroy(&(parameters.syncSem));
	sem_destroy(&(parameters.doneSem));

	// Create a return packet from the handler response and the original query
	FSWPacket * ret = new FSWPacket(packet->GetDestination(), packet->GetSource(), packet->GetTimestamp(),
			parameters.retPacket->GetOpcode(), parameters.retPacket->IsSuccess(), true, parameters.retPacket->GetType(),
			parameters.retPacket->GetMessageLength(), parameters.retPacket->GetMessageBufPtr());

	// FIXME: make sure this doesn't cause a memory leak!
	delete parameters.retPacket;
	delete packet;

	// Send the response back to the server that dispatched the original message.
	while (!mq_timed_send(queueNameRX, &ret, MAX_BLOCK_TIME, 0));
	return true;
}

bool Dispatcher::IsPacketMatchingResponse(const FSWPacket & packetIn,
		const FSWPacket & packetOut) const
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("IsPacketMatchingResponse result: %d", ((packetOut.IsResponse())
			&& (packetOut.GetDestination( ) == packetIn.GetSource( ))
			&& (packetOut.GetNumber( ) == packetIn.GetNumber( ))), LOGGER_LEVEL_DEBUG);

	logger->Log("IsPacketMatchingResponse response result: %d", packetOut.IsResponse(), LOGGER_LEVEL_DEBUG);
	logger->Log("IsPacketMatchingResponse dest/source: %d", packetOut.GetDestination( ) == packetIn.GetSource( ), LOGGER_LEVEL_DEBUG);
	logger->Log("IsPacketMatchingResponse number: %d", packetOut.GetNumber( ) == packetIn.GetNumber( ), LOGGER_LEVEL_DEBUG);


	//	DEBUG_PRINT("Original FSWPacket - ", (&packetIn));
	//	DEBUG_PRINT("Queue FSWPacket    - ", (&packetOut));
	//debug_led_set_led(5, LED_ON);
	// Return true if *packetOut is a response to *packetIn.
	return ((packetOut.IsResponse())
			&& (packetOut.GetDestination( ) == packetIn.GetSource( ))
			&& (packetOut.GetNumber( ) == packetIn.GetNumber( )));
}

bool Dispatcher::IsPacketDestMatchingSource(const FSWPacket & packetIn,
		const FSWPacket & packetOut) const
{
	// Return true if *packetOut is being sent to the server in
	// packetIn->GetSource().
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("IsPacketDestMatchingSource() result: %d", ((!packetOut.IsResponse( ))
					&& (packetOut.GetDestination( ) == packetIn.GetSource( ))), LOGGER_LEVEL_DEBUG);
	return ((!packetOut.IsResponse( ))
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("Dispatcher: CheckQueueForMatchingPacket(): Called", LOGGER_LEVEL_DEBUG);

	// Get the semaphore
	if (true == this->TakeLock(MAX_BLOCK_TIME))
	{
		// Check for the first message in the queue

		if (mq_timed_receive(queueNameRX, &packetOut, 0, DISPATCHER_MAX_DELAY) == false)
		{
			this->GiveLock();
			return false;
		}
		else
		{
			//debug_led_set_led(2, LED_ON);
			// There's at least one packet, so check if it matches packetIn.

			logger->Log("Dispatcher: CheckQueueForMatchingPacket(): There is at least one packet", LOGGER_LEVEL_DEBUG);
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
				logger->Log("Dispatcher: CheckQueueForMatchingPacket(): checking more packets", LOGGER_LEVEL_DEBUG);
				numPackets = mq_size(queueHandleRX, queueAttrRX);
				logger->Log("Dispatcher: CheckQueueForMatchingPacket(): there are %u more packets", (uint32) numPackets, LOGGER_LEVEL_DEBUG);

				// Get each packet and check it against packetIn.
				for (i = 0; i < numPackets; ++i)
				{
					// Get the next packet.
					//mqd_t tmpqueueHandle = open(queueName, O_RDONLY);
					if (mq_timed_receive(queueNameRX, &tmpPacket, 0, 0)
							== false)
					{
						//debug_led_set_led(4, LED_ON);
						// Error: There should have been a packet in
						// the queue, but there wasn't, so put
						// packetOut back.
						if (mq_timed_send(queueNameRX, &packetOut, 1, 0)
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
					if (mq_timed_send(queueNameRX, &packetOut, 1, 0)
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
				if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false)
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
		logger->Log("Dispatcher: CheckQueueForMatchingPacket(): Sem Failed - ", LOGGER_LEVEL_ERROR);
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

		while (!mq_timed_send(queueNameRX, &packet, MAX_BLOCK_TIME, 0));
		return false;
	}
	src = packet->GetSource( );
	packet->SetSource(packet->GetDestination( ));
	packet->SetDestination(src);
	//packet->SetMessage(ret);
	packet->SetMessageBuf(NULL);
	delete ret;
	while (!mq_timed_send(queueNameRX, &packet, MAX_BLOCK_TIME, 0));
	return true;
}

void * Dispatcher::InvokeHandler(void * parameters)
{
	DispatcherTaskParameter * parms =
			(DispatcherTaskParameter *) parameters;

	xSemaphoreTake(&(parms->doneSem), MAX_BLOCK_TIME, 0);
	sem_post(&(parms->syncSem));
	parms->retPacket = parms->registry->Invoke(*(parms->packet));


	sem_post(&(parms->doneSem));

	pthread_exit(0);
}

uint32_t Dispatcher::DispatchToHardware( FSWPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("Dispatcher: DispatchtoHardware() called", LOGGER_LEVEL_DEBUG);
	//todo: add semaphores for locking & real error values
	ssize_t packetLength;
	uint8_t * packetBuffer;
	ssize_t bytesCopied;
	size_t iterations;
	bool timedOut;
	int protocolChoice = -1;

	packetLength = packet.GetFlattenSize();
	logger->Log("DispatchtoHardware(): dispatch packet size %d", (uint32) packetLength, LOGGER_LEVEL_DEBUG);

//	pthread_t self_id;
//	self_id = pthread_self();
//	printf("DispatchToHardware: Thread %u\n", self_id);

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
		logger->Log("DispatchToHardware(): flatten failed", LOGGER_LEVEL_WARN);
		return -2;
	}

	for(uint16_t i = 0; i < packetLength; i++)
	{
		printf("0x%02x ",packetBuffer[i]);
	}
	printf("\n");

	// Get the instances for the Ethernet and SPI Servers
	ETH_HALServer * eth_server = dynamic_cast<ETH_HALServer *> (Factory::GetInstance(ETH_HALSERVER_SINGLETON));
	SPI_HALServer * spi_server = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	// Protocol Switching Code
	Servers::CMDServer * cmdServer = dynamic_cast<Servers::CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	protocolChoice = cmdServer->subsystem_acp_protocol[packet.GetDestination()];

	FSWPacket * txPacket = &packet;
	switch(protocolChoice){
		case ACP_PROTOCOL_SPI:
			//logger->Log("DispatchToHardware(): Dispatch over SPI", LOGGER_LEVEL_DEBUG);
			while (!mq_timed_send(spi_server->queueNameSPITX, NULL, MAX_BLOCK_TIME, 0));
			break;
		case ACP_PROTOCOL_ETH:
			logger->Log("DispatchToHardware(): Dispatch over SPI", LOGGER_LEVEL_DEBUG);
			bytesCopied = eth_server->ETHDispatch(packet);
			break;
		default:
			//log error
			break;
	}

	if(bytesCopied != packetLength){
		//todo error handling
	}

	/*
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
	*/

	return 0;
}

Dispatcher::Dispatcher(void)
		: Singleton(), registryMap()
{
	mq_unlink(queueNameRX);
	qInitRX = mqCreate(&queueHandleRX, &queueAttrRX, queueNameRX);
}

Dispatcher::Dispatcher(const Dispatcher & source)
{
	// Left Intentionally Empty
}

#ifdef TEST
void Dispatcher::Destroy(void)
{
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
	mq_unlink(queueNameRX);
}

Dispatcher & Dispatcher::operator=(const Dispatcher & source)
{
	return *this;
}

    }
}


