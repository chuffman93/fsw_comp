/*! \file Dispatcher.cpp
 *  \brief Implementation File for the Dispatcher Class
 *
 *  This file contains the implementation of the Dispatcher class, which acts
 *  as the ACPPacket-routing entity in the AllStar architecture.
 */

#include <utility>

#include "core/Dispatcher.h"
#include "core/Factory.h"
#include "core/Singleton.h"
#include "core/WatchdogManager.h"
#include "util/Logger.h"

#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <string>
#include "../src/HAL/Ethernet_Server.h"
#include "HAL/SPI_Server.h"
#include "servers/CMDServer.h"
#include "POSIX.h"

using namespace std;

namespace AllStar{
namespace Core{

// Instantiate static members
char * Dispatcher::queueNameRX = (char *) "/queueHandleRX";
uint16 Dispatcher::packetNumber = 1;
size_t timer;

Dispatcher::Dispatcher(void)
		: Singleton(), registryMap()
{
	mq_unlink(queueNameRX);
	qInitRX = mqCreate(&queueHandleRX, &queueAttrRX, queueNameRX);
}

#ifdef TEST
void Dispatcher::Destroy(void)
{
	mq_unlink(queueNameRX);
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

void Dispatcher::Initialize(void)
{
	// Intentionally left empty
}

bool Dispatcher::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized() && (qInitRX == 0));
}

bool Dispatcher::AddRegistry(LocationIDType serverID,
		MessageHandlerRegistry * registry, Arbitrator * arby)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	pair<IteratorType, bool> ret;

	if (NULL == registry || NULL == arby)
	{
		return false;
	}

	// get the semaphore.
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
		logger->Log("Dispatcher: AddRegistry(): Semaphore failed", LOGGER_LEVEL_WARN);
		return false;
	}
}

bool Dispatcher::Dispatch(ACPPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	if(packet.getDestination() < HARDWARE_LOCATION_MIN || packet.getDestination() > SERVER_LOCATION_MAX){
		return false;
	}
	if(packet.getDestination() >= HARDWARE_LOCATION_MIN && packet.getDestination() < HARDWARE_LOCATION_MAX)
	{
		logger->Log("---- Hardware Dispatch ----", LOGGER_LEVEL_DEBUG);
		uint32_t ret;
		ret=DispatchToHardware(packet);
		return (0 == ret);
	}else{
		logger->Log("Now sending the message to the queue", LOGGER_LEVEL_DEBUG);

		if (true == this->TakeLock(MAX_BLOCK_TIME))
		{
			logger->Log("Dispatcher: Took lock", LOGGER_LEVEL_DEBUG);
			size_t numPackets = mq_size(queueHandleRX, queueAttrRX);
			ACPPacket * tmpPacket;
			try
			{
				tmpPacket = new ACPPacket(packet);
			}
			catch (bad_alloc & e)
			{
				this->GiveLock();
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
				logger->Log("Dispatcher::Dispatch() Queue is full", LOGGER_LEVEL_WARN);
				delete tmpPacket;
				this->GiveLock();
				return false;
			}
			this->GiveLock();
			return false;
		}
		else
		{
			logger->Log("Dispatch(): Semaphore failed", LOGGER_LEVEL_WARN);
			return false;
		}
	}
}

bool Dispatcher::CheckQueueForMatchingPacket(const ACPPacket & packetIn, ACPPacket * &packetOut, DispatcherCheckType type)
{
	size_t numPackets, i;
	ACPPacket * tmpPacket;
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	PacketCheckFunctionType Check;
	//logger->Log("Dispatcher: CheckQueueForMatchingPacket(): Called", LOGGER_LEVEL_DEBUG);

	switch(type){
		case CHECK_MATCHING_RESPONSE:
			Check = &Dispatcher::IsPacketMatchingResponse;
			break;
		case CHECK_DEST_SOURCE:
			Check = &Dispatcher::IsPacketDestMatchingSource;
			break;
		case CHECK_SAME:
			Check = &Dispatcher::IsPacketSame;
			break;
		default:
			break;
	}

	// get the semaphore
	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		// Check for the first message in the queue
		if (mq_timed_receive(queueNameRX, &packetOut, 0, DISPATCHER_MAX_DELAY) == false){
			this->GiveLock();
			return false;
		}else{
			// There's at least one packet, so check if it matches packetIn.

			logger->Log("Dispatcher: CheckQueueForMatchingPacket(): There is at least one packet", LOGGER_LEVEL_SUPER_DEBUG);
			if(packetOut == NULL){
				this->GiveLock();
				return false;
			}

			if ((this->*Check)(packetIn, *packetOut)){
				this->GiveLock();
				return true;
			}else{
				// Check the number of packets waiting in the queue.
				logger->Log("Dispatcher: CheckQueueForMatchingPacket(): checking more packets", LOGGER_LEVEL_SUPER_DEBUG);
				numPackets = mq_size(queueHandleRX, queueAttrRX);
				logger->Log("Dispatcher: CheckQueueForMatchingPacket(): there are %u more packets", (uint32) numPackets, LOGGER_LEVEL_SUPER_DEBUG);

				// get each packet and check it against packetIn.
				for (i = 0; i < numPackets; ++i){
					// get the next packet.
					//mqd_t tmpqueueHandle = open(queueName, O_RDONLY);
					if (mq_timed_receive(queueNameRX, &tmpPacket, 0, 0) == false){
						// Error: There should have been a packet in
						// the queue, but there wasn't, so put
						// packetOut back.
						if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false){
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
					if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false){
						// Error
						this->GiveLock();
						throw(2);
						break;
					}
					packetOut = tmpPacket;

					// Check if packetOut matches packetIn
					if ((this->*Check)(packetIn, *packetOut)){
						// Found one!  Time to return!
						this->GiveLock();
						return true;
					}

				}
				// No packets were found, so put the last packet
				// back on the queue.
				if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false){
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

MessageHandlerRegistry * Dispatcher::FindHandler(LocationIDType serverID, ACPPacket * packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	IteratorType it;

	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		logger->Log("   Dispatcher: searching registry map for handler.", LOGGER_LEVEL_DEBUG);
		it = registryMap.find(serverID);
		this->GiveLock();
	}else{
		delete packet;
		return NULL;
	}

	if (registryMap.end( ) == it){
		logger->Log("   Dispatcher: Listen(): Didn't find a handler.", LOGGER_LEVEL_DEBUG);
		delete packet;
		return NULL;
	}

	// A handler exists, so check the permissions
	logger->Log("   Dispatcher: Listen(): Handler exists, checking permissions.", LOGGER_LEVEL_DEBUG);

	ArbitratorAuthStatusEnum arbyRet;
	if (ARBITRATOR_AUTH_STATUS_PERMISSION != (arbyRet = it->second->arby->Authenticate(*packet))){
		logger->Log("   Dispatcher: Listen(): Don't have permissions, reject packet.", LOGGER_LEVEL_ERROR);
		logger->Log("   Dispatcher: Listen(): Authenticate returned: %d", arbyRet, LOGGER_LEVEL_ERROR);
		delete packet;
		return NULL;
	}

	// Permissions are correct
	return it->second->registry;

}

bool Dispatcher::IsPacketMatchingResponse(const ACPPacket & packetIn,
		const ACPPacket & packetOut) const
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("IsPacketMatchingResponse result: %d", ((packetOut.isFromHardware())
			&& (packetOut.getPacketID( ) == packetIn.getPacketID( ))), LOGGER_LEVEL_SUPER_DEBUG);

	logger->Log("IsPacketMatchingResponse response result: %d", packetOut.isFromHardware(), LOGGER_LEVEL_SUPER_DEBUG);
	logger->Log("IsPacketMatchingResponse source/dest: %d", packetOut.getSource() == packetIn.getDestination(), LOGGER_LEVEL_SUPER_DEBUG);
	logger->Log("IsPacketMatchingResponse number: %d", packetOut.getPacketID( ) == packetIn.getPacketID( ), LOGGER_LEVEL_SUPER_DEBUG);

	// Return true if *packetOut is a response to *packetIn.
	return ((packetOut.isFromHardware())
			&& (packetOut.getSource() == packetIn.getDestination())
			&& (packetOut.getPacketID() == packetIn.getPacketID()));
}

bool Dispatcher::IsPacketDestMatchingSource(const ACPPacket & packetIn,
		const ACPPacket & packetOut) const
{
	// Return true if *packetOut is being sent to the server in
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("IsPacketDestMatchingSource() result: %d", ((!packetOut.isFromHardware( ))
					&& (packetOut.getDestination( ) == packetIn.getSource( ))), LOGGER_LEVEL_SUPER_DEBUG);
	return ((!packetOut.isFromHardware( ))
			&& (packetOut.getDestination( ) == packetIn.getSource( )));
}

bool Dispatcher::IsPacketSame(const ACPPacket & packetIn,
		const ACPPacket & packetOut) const
{
	// Return true if packetIn and packetOut are equivalent.
	return packetIn == packetOut;
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

uint32_t Dispatcher::DispatchToHardware(ACPPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("Dispatcher: DispatchtoHardware() called, packet number: %d", packet.getPacketID(), LOGGER_LEVEL_DEBUG);
	//todo: add semaphores for locking & real error values
	size_t packetLength;
	uint8_t * packetBuffer;
	size_t bytesCopied;
	int protocolChoice = -1;

	packetLength = packet.GetFlattenSize();
	logger->Log("DispatchtoHardware(): dispatch packet size %d", (uint32) packetLength, LOGGER_LEVEL_DEBUG);

	if(packetLength >= MAX_PACKET_SIZE)
	{
		logger->Log("DispatchtoHardware(): packet too large!", LOGGER_LEVEL_ERROR);
		return -1;
	}

	packetBuffer = (uint8_t *) malloc(packetLength);

	//check if whole packet was copied
	if(packet.Flatten(packetBuffer,packetLength) != packetLength)
	{
		logger->Log("DispatchToHardware(): flatten failed", LOGGER_LEVEL_WARN);
		return -2;
	}

	for(uint16_t i = 0; i < packetLength; i++)
	{
		printf("0x%02x ",packetBuffer[i]);
	}
	printf("\n");

	// get the instances for the Ethernet and SPI Servers
	//ETH_HALServer * eth_server = dynamic_cast<ETH_HALServer *> (Factory::GetInstance(ETH_HALSERVER_SINGLETON));
	SPI_HALServer * spi_server = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	Servers::CMDServer * cmdServer = dynamic_cast<Servers::CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	protocolChoice = cmdServer->subsystem_acp_protocol[packet.getDestination()];

	ACPPacket * txPacket = &packet;
	bool sendSuccess = false;
	size_t numPackets;
	switch(protocolChoice){
		case ACP_PROTOCOL_SPI:
			if(true == this->TakeLock(MAX_BLOCK_TIME)){
				numPackets = mq_size(spi_server->queueHandleTX, spi_server->queueAttrTX);
				if(numPackets < DISPATCHER_QUEUE_LENGTH){ // same length as all queues
					logger->Log("Dispatcher: Placing message on SPI queue", LOGGER_LEVEL_INFO);
					sendSuccess = mq_timed_send(spi_server->queueNameSPITX, &txPacket, MAX_BLOCK_TIME, 0);
					this->GiveLock();
				}else{
					logger->Log("Dispatcher: SPI TX Queue full!", LOGGER_LEVEL_FATAL);
					sendSuccess = false;
					this->GiveLock();
				}
			}else{
				logger->Log("DispatchToHardware: Semaphore failed!", LOGGER_LEVEL_WARN);
				sendSuccess = false;
			}
			break;
		case ACP_PROTOCOL_ETH:
			logger->Log("DispatchToHardware(): Dispatch over ETH", LOGGER_LEVEL_DEBUG);
//			bytesCopied = eth_server->ETHDispatch(packet);
//			sendSuccess = (bytesCopied == packet.GetFlattenSize());
			break;
		default:
			logger->Log("DispatchToHardware(): ACP Protocol out of bounds!", LOGGER_LEVEL_ERROR); // TODO: assert?
			sendSuccess = false;
			break;
	}

	if(sendSuccess){
		return 0;
	}else{
		return -3;
	}
}

void Dispatcher::CleanRXQueue(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ACPPacket * iterator;
	size_t numPackets;

	uint64 thresholdTime = getTimeInMillis() - 15000;
	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		numPackets = mq_size(queueHandleRX, queueAttrRX);
		if(numPackets == 0){
			// no packets to check, just return
			this->GiveLock();
			return;
		}else{
			for(size_t i = 0; i < numPackets; i++){
				if(mq_timed_receive(queueNameRX, &iterator, 0, DISPATCHER_MAX_DELAY) == false){
					logger->Log("Dispatcher::CleanRXQueue(): error getting packet", LOGGER_LEVEL_WARN);
				}else{
					if(iterator->getTimestamp() < thresholdTime){
						// packet is too old, delete?
						logger->Log("Dispatcher::CleanRXQueue(): deleting old packet from queue", LOGGER_LEVEL_WARN);
						delete iterator;
					}else{
						// packet OK, so place it back on the queue
						if(mq_timed_send(queueNameRX, &iterator, 1, 0) == false){
							logger->Log("Dispatcher::CleanRXQueue(): Failed to replace message on the RX queue", LOGGER_LEVEL_WARN);
						}
					}
				}
			}
			this->GiveLock();
		}
	}else{
		logger->Log("Dispatcher: Can't take lock", LOGGER_LEVEL_WARN);
	}
}

} // End of namespace Core
} // End of namespace AllStar
