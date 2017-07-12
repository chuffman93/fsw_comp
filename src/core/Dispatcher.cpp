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
#include "HAL/SPI_Server.h"
#include "servers/CMDServer.h"
#include "POSIX.h"

using namespace std;

namespace AllStar {
namespace Core {

// Instantiate static members
char * Dispatcher::queueNameRX = (char *) "/queueHandleRX";
uint16 Dispatcher::packetNumber = 1;
size_t timer;

Dispatcher::Dispatcher(void)
		: Singleton() {
	mq_unlink(queueNameRX);
	qInitRX = mqCreate(&queueHandleRX, &queueAttrRX, queueNameRX);
}

Dispatcher::~Dispatcher(void) {
	mq_unlink(queueNameRX);
}

Dispatcher & Dispatcher::operator=(const Dispatcher & source) {
	return *this;
}

bool Dispatcher::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized() && (qInitRX == 0));
}

bool Dispatcher::CheckQueueForMatchingPacket(const ACPPacket & packetIn, ACPPacket * &packetOut, DispatcherCheckType type) {
	size_t numPackets, i;
	ACPPacket * tmpPacket;
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	PacketCheckFunctionType Check;

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
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		// Check for the first message in the queue
		if (mq_timed_receive(queueNameRX, &packetOut, 0, DISPATCHER_MAX_DELAY) == false) {
			this->GiveLock();
			return false;
		} else {
			logger->SuperDebug("Dispatcher: CheckQueueForMatchingPacket(): There is at least one packet");
			if (packetOut == NULL) {
				this->GiveLock();
				return false;
			}

			if ((this->*Check)(packetIn, *packetOut)) {
				this->GiveLock();
				return true;
			} else {
				// Check the number of packets waiting in the queue.
				logger->SuperDebug("Dispatcher: CheckQueueForMatchingPacket(): checking more packets");
				numPackets = mq_size(queueHandleRX, queueAttrRX);
				logger->SuperDebug("Dispatcher: CheckQueueForMatchingPacket(): there are %u more packets", (uint32) numPackets);

				// get each packet and check it against packetIn.
				for (i = 0; i < numPackets; ++i){
					if (mq_timed_receive(queueNameRX, &tmpPacket, 0, 0) == false){
						if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false) { // Error: This is really bad because we can't even preserve the state of the queue.
							this->GiveLock();
							break;
						}
						this->GiveLock();
						break;
					}

					// Put packetOut back on the queue since it didn't match packetIn.
					if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false) {
						// Error
						this->GiveLock();
						break;
					}
					packetOut = tmpPacket;

					// Check if packetOut matches packetIn
					if ((this->*Check)(packetIn, *packetOut)) {
						this->GiveLock();
						return true;
					}
				}

				// No packets were found, so put the last packet back on the queue
				if (mq_timed_send(queueNameRX, &packetOut, 1, 0) == false) {
					this->GiveLock();
				}
				this->GiveLock();
				return false;
			}
		}
	} else {
		logger->Error("Dispatcher: CheckQueueForMatchingPacket(): Sem Failed - ");
		return false;
	}
}

bool Dispatcher::IsPacketMatchingResponse(const ACPPacket & packetIn, const ACPPacket & packetOut) const {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->SuperDebug("IsPacketMatchingResponse result: %d", ((packetOut.isFromHardware())
			&& (packetOut.getPacketID( ) == packetIn.getPacketID( ))));

	logger->SuperDebug("IsPacketMatchingResponse response result: %d", packetOut.isFromHardware());
	logger->SuperDebug("IsPacketMatchingResponse source/dest: %d", packetOut.getSource() == packetIn.getDestination());
	logger->SuperDebug("IsPacketMatchingResponse number: %d", packetOut.getPacketID( ) == packetIn.getPacketID( ));

	// Return true if *packetOut is a response to *packetIn.
	return ((packetOut.isFromHardware())
			&& (packetOut.getSource() == packetIn.getDestination())
			&& (packetOut.getPacketID() == packetIn.getPacketID()));
}

bool Dispatcher::IsPacketDestMatchingSource(const ACPPacket & packetIn, const ACPPacket & packetOut) const {
	return ((!packetOut.isFromHardware( )) && (packetOut.getDestination( ) == packetIn.getSource( )));
}

bool Dispatcher::IsPacketSame(const ACPPacket & packetIn, const ACPPacket & packetOut) const {
	return packetIn == packetOut;
}

bool Dispatcher::Dispatch(ACPPacket & packet) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Debug("Dispatcher: Dispatch, packet number: %d", packet.getPacketID());

	// check bounds
	if (packet.getDestination() < HARDWARE_LOCATION_MIN || packet.getDestination() >= HARDWARE_LOCATION_MAX) {
		logger->Error("Dispatcher: invalid dispatch location");
		return false;
	}

	//todo: add semaphores for locking & real error values
	size_t packetLength;
	uint8_t * packetBuffer;

	packetLength = packet.GetFlattenSize();
	logger->Debug("DispatchtoHardware(): dispatch packet size %d", (uint32) packetLength);

	if (packetLength >= MAX_PACKET_SIZE) {
		logger->Error("DispatchtoHardware(): packet too large!");
		return -1;
	}

	packetBuffer = (uint8_t *) malloc(packetLength);

	switch(packet.getDestination()) {
	case HARDWARE_LOCATION_EPS:
		packet.setSync(SYNC_VALUE | 0x00);
		break;
	case HARDWARE_LOCATION_COM:
		packet.setSync(SYNC_VALUE | 0x01);
		break;
	case HARDWARE_LOCATION_ACS:
		packet.setSync(SYNC_VALUE | 0x02);
		break;
	case HARDWARE_LOCATION_PLD:
		packet.setSync(SYNC_VALUE | 0x03);
		break;
	default:
		logger->Warning("DispatchToHardware: bad destination!");
		return false;
	}

	//check if whole packet was copied
	if (packet.Flatten(packetBuffer,packetLength) != packetLength) {
		logger->Warning("DispatchToHardware(): flatten failed");
		return false;
	}

	SPI_HALServer * spi_server = static_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	ACPPacket * txPacket = &packet;
	bool sendSuccess = false;
	size_t numPackets;

	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		numPackets = mq_size(spi_server->queueHandleTX, spi_server->queueAttrTX);

		if (numPackets < DISPATCHER_QUEUE_LENGTH) { // same length as all queues
			logger->Debug("Dispatcher: Placing message on SPI queue");
			sendSuccess = mq_timed_send(spi_server->queueNameSPITX, &txPacket, MAX_BLOCK_TIME, 0);
			this->GiveLock();
		} else {
			logger->Fatal("Dispatcher: SPI TX Queue full!");
			this->GiveLock();
			return false;
		}
	} else {
		logger->Warning("DispatchToHardware: Semaphore failed!");
		return false;
	}

	return true;
}

void Dispatcher::CleanRXQueue() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ACPPacket * iterator;
	size_t numPackets;

	uint64 thresholdTime = getTimeInMillis() - 15000;
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		numPackets = mq_size(queueHandleRX, queueAttrRX);
		if (numPackets == 0) {
			// no packets to check, just return
			this->GiveLock();
			return;
		} else {
			for (size_t i = 0; i < numPackets; i++) {
				if (mq_timed_receive(queueNameRX, &iterator, 0, DISPATCHER_MAX_DELAY) == false) {
					logger->Warning("Dispatcher::CleanRXQueue(): error getting packet");
				} else {
					if (iterator->getTimestamp() < thresholdTime) {
						// packet is too old, delete?
						logger->Warning("Dispatcher::CleanRXQueue(): deleting old packet from queue");
						delete iterator;
					} else {
						// packet OK, so place it back on the queue
						if(mq_timed_send(queueNameRX, &iterator, 1, 0) == false){
							logger->Warning("Dispatcher::CleanRXQueue(): Failed to replace message on the RX queue");
						}
					}
				}
			}
			this->GiveLock();
		}
	} else {
		logger->Warning("Dispatcher: Can't take lock");
	}
}

} // End of namespace Core
} // End of namespace AllStar
