/*
 * SPI_Server.cpp
 *
 *	Created: March 2016
 *		Author: Connor Kelleher
 *
 *	Updated: June, 2016
 *		Authors: Alex St. Clair, Robert Belter
 */

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "HAL/SPI_Server.h"
#include "core/ACPPacket.h"
#include "core/Dispatcher.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include "servers/SubsystemServer.h"

#define STR_LEN	   50

using namespace AllStar;
using namespace Core;
using namespace std;

static int timeout = 3000;

char * SPI_HALServer::queueNameSPITX = (char *) "/queueHandleSPITX";

SPI_HALServer::SPI_HALServer()
	: AllStar::Servers::SubsystemServer("SPI", SERVER_LOCATION_SPI), packetsSentTX(0), packetsDroppedTX(0) {
	mq_unlink(queueNameSPITX);
	qInitTX = mqCreate(&queueHandleTX, &queueAttrTX, queueNameSPITX);
}

SPI_HALServer::~SPI_HALServer() {
	mq_unlink(queueNameSPITX);
}

// Enter this loop
void SPI_HALServer::SubsystemLoop(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ACPPacket * rxPacket;
	struct pollfd fds;
	int64 enterTime;
	int slave_fd;
	int nbytes;
	int queueSize;
	uint8 * rxBuf;
	uint8 clearBuf;
	bool queueSuccess;

	// Setup the GPIOs for SPI
	GPIOsetup();

	logger->Info("*****************************************************");
	logger->Info("SPI_HAL Server: Entering Loop --------------------- *");
	logger->Info("*****************************************************");

	while (1) {
		errno = 0;
		wdmAlive();
		enterTime = getTimeInMillis();

		// ---- TX ----------------------------------------------------------------------------------------------------------------
		ACPPacket * txPacket;
		if (mq_size(queueHandleTX, queueAttrTX) > 0) {
			if (mq_timed_receive(queueNameSPITX, &txPacket, 0, DISPATCHER_MAX_DELAY)) {
				LocationIDType dest = txPacket->getDestination();
				if (0 < dest && dest < 5) {
					packetsSentTX++;
					if (SPIDispatch(*txPacket)) {
						logger->Debug("SPI_HAL Server: Successfully dispatched packet");
						logTXSuccess(dest, txPacket->getOpcode());
					} else {
						logger->Warning("SPI_HAL Server: " "\x1b[33m" "Packet dispatch failed!" "\x1b[0m" " Subsystem: %d", txPacket->getDestination());
						logTXFailure(dest, txPacket->getOpcode());
						// increment dropped packets counter for that subsystem
						packetsDroppedTX++;
					}
				} else {
					logger->Warning("SPI_HALServer: Invalid TX destination!");
				}
			} else {
				logger->Warning("SPI_HAL Server: Queue receive for TX failed!");
			}
		}

		// ---- RX ----------------------------------------------------------------------------------------------------------------
		for (int i = 0; i < NUM_SLAVES; i++) {
			poll(poll_fds+i, 1, 0); // quick timeout (only checking for events, not waiting)
			read(int_fds[i], &clearBuf, 1);
			if (poll_fds[i].revents & POLLPRI) {
				logger->Debug("SPI_HAL Server: found interrupt on fds %d", i);
				slave_fd = spi_fds[i];
				fds.fd = int_fds[i];
				fds.events = POLLPRI;
				nbytes = spi_read(slave_fd, &fds, &rxBuf);
				if (nbytes < ACP_MIN_BYTES) {
					rxPacket = NULL;
				} else {
					rxPacket = new ACPPacket(rxBuf, nbytes);
					set_packet_sourcedest(i, rxPacket);
				}

				// Check bounds and send to dispatcher RX queue
				if (rxPacket == NULL) {
					logger->Error("SPI_HAL Server: " "\x1b[33m" "There was an error reading the packet." "\x1b[0m" " Not placing on queue!");
				} else if (rxPacket->getDestination() == LOCATION_ID_INVALID) {
					logger->Warning("SPI_HAL Server: ACP Packet dest invalid (bit flip). Not placing on queue!");
					delete rxPacket;
				} else {
					Dispatcher * dispatcher = static_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
					queueSize = mq_size(dispatcher->queueHandleRX, dispatcher->queueAttrRX);
					if (queueSize < DISPATCHER_QUEUE_LENGTH) {
						logger->Debug("SPI_Server: placing RX message on dispatcher RX queue");
						queueSuccess = mq_timed_send(dispatcher->queueNameRX, &rxPacket, MAX_BLOCK_TIME, 0);
					} else {
						logger->Fatal("SPI_Server: RX queue full!");
					}
				}
			}
		}
		waitUntil(enterTime, 5); // wait 5 ms
	}
}

bool SPI_HALServer::SPIDispatch(AllStar::Core::ACPPacket & packet) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int bytes_copied;
	int ret;
	int slave_fd;
	ssize_t packetLength;
	uint8_t * packetBuffer;
	struct pollfd fds;
	int destination = 0;
	packetLength = packet.GetFlattenSize();
	logger->Warning("SPI_Server: SPIDispatch(): Dispatching packet of size %d", (uint32) packetLength);
	logger->Warning("SPI_Server: SPIDispatch(): Opcode %u", (uint32) packet.getOpcode());
	logger->Warning("SPI_Server: SPIDispatch(): Dispatch to %d", packet.getDestination());

	if (packetLength >= MAX_PACKET_SIZE) {
		logger->Error("SPI_Server: SPIDispatch(): Packet too large!");
		return -1;
	}

	packetBuffer = (uint8_t *) malloc(packetLength);
	ret = packet.Flatten(packetBuffer, packetLength);

	//check if whole packet was copied
	if (ret != packetLength) {
		logger->Warning("SPI_Server: SPIDispatch(): Packet flatten fail");
		return -2;
	}

	destination = packet.getDestination();
	logger->Warning("SPI_Server: SPIDispatch(): destination = %d", destination);
	get_int_fds(destination, &fds);

	slave_fd = get_slave_fd(destination);
	logger->Warning("SPI_Server: SPIDispatch(): Writing packet to SPI");
	cout << "PacketLength: " << packetLength << endl;
	bytes_copied = this->spi_write(slave_fd, &fds, packetBuffer, packetLength);
	logger->Warning("SPI_Server: SPIDispatch(): Waiting on return message");
	return (bytes_copied == packetLength);
}

int SPI_HALServer::spi_write(int slave_fd, struct pollfd * fds, uint8_t* buf, int len) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int buf_pt = 0;
	int wr_size = 1;
	int ret;
	uint8_t clearBuf;
	read(fds->fd, &clearBuf, 1);
	while (buf_pt != len) {
		logger->SuperDebug("Writing byte %d", buf_pt);
		logger->Debug("Byte value: ----------------------- 0x%x", *(buf + buf_pt));
		ret = write(slave_fd, buf + buf_pt, wr_size);

		if (ret != wr_size) {
			logger->Warning("spi_write: Failed to write byte!");
			printf("Ret: %d", ret);
			perror("Error sending byte");
			return -1;
		}
		buf_pt++;

		//Wait for interrupt before sending next byte
		logger->SuperDebug("Waiting for interrupt after byte send");
		logger->SuperDebug("spi_write: Polling");
		int retval = poll(fds, 1, timeout);
		cout << "fds struct variables:  " << fds->fd << " " << fds->events << " " << fds->revents << endl;

		if (!(fds->revents & POLLPRI) || retval == 0) {
			logger->Error("Poll timeout!");
			return -1;
		}
		logger->SuperDebug("spi_write: Poll return: %d", retval);
		this->GiveLock();
		logger->SuperDebug("spi_write: After poll");
		ret = read(fds->fd, &clearBuf, 1);
		logger->SuperDebug("spi_write: revent = 0x%x", fds->revents);
		if (fds->fd < 0) {
			perror("Err opening\n");
		}
	}
	return buf_pt;
}

int SPI_HALServer::spi_read(int slave_fd, struct pollfd * fds, uint8 **rx_bufp) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int buf_pt = 0;
	int i;
	int fd = fds->fd;
	int rx_complete = 0;
	int retval;
	uint8_t clearBuf;
	uint16_t packet_len = 0;

	struct spi_ioc_transfer tr;

	uint8 *rx_buf;

	tr.tx_buf = 0;
	tr.rx_buf = 0;
	tr.len = 1;
	tr.delay_usecs = 0;
	tr.speed_hz = 1000000;
	tr.cs_change = 0;
	tr.bits_per_word = 8;

	uint8 rx, tx = 0;

	rx_buf = (uint8 *) malloc(sizeof(uint8) * 16);

	read(fds->fd, &clearBuf, 1);
	while (rx_complete != true) {

		for (i = 0; i < 6; i++) {
			//Wait for interrupt before sending next byte
			if (i != 0) {
				logger->SuperDebug("spi_read: Waiting for interrupt");
				retval = poll(fds, 1, timeout);
				if (!(fds->revents & POLLPRI) || retval == 0) {
					logger->Warning("Poll timeout!");
					return 0;
				}
			}

			read(fds->fd, &clearBuf, 1);
			logger->SuperDebug("spi_read: Reading byte %d: ", buf_pt);
			tr.tx_buf =  (unsigned long) &tx;
			tr.rx_buf =  (unsigned long) &rx;

			ioctl(slave_fd, SPI_IOC_MESSAGE(1), &tr);
			rx_buf[buf_pt] = rx;

			logger->Debug("spi_read: byte value: ------------- 0x%X", rx);
			buf_pt++;

			if (i == 0 && (rx & 0xFC) != SYNC_VALUE) {
				logger->Warning("spi_read: sync byte incorrect! Ending read");
				return 0;
			}
		}

		logger->Debug("spi_read: Reallocating memory for packet + message");
		//Allocate more memory based on length
		packet_len = (rx_buf[4] << 8 | rx_buf[5]) + 8;
		logger->Debug("spi_read: message size = %d", packet_len - 8);
		rx_buf = (uint8 *) realloc(rx_buf, packet_len * sizeof(uint8));

		//Read in remaining bytes
		for (i = 0; i < packet_len - 6; i++) {
			//Wait for interrupt before sending next byte
			logger->Warning("spi_read: Waiting for interrupt");
			retval = poll(fds, 1, timeout);
			if (!(fds->revents & POLLPRI) || retval == 0) {
				logger->Error("Poll timeout!");
				return 0;
			}
			read(fds->fd, &clearBuf, 1);
			logger->SuperDebug("spi_read: Reading byte %d: ", buf_pt);
			tr.tx_buf =  (unsigned long) &tx;
			tr.rx_buf =  (unsigned long) &rx;

			ioctl(slave_fd, SPI_IOC_MESSAGE(1), &tr);
			rx_buf[buf_pt] = rx;

			logger->Debug("spi_read: byte value: ------------- 0x%x", rx);
			buf_pt++;
		}

		*rx_bufp = rx_buf;
		return buf_pt;
	}
	return 0;
}

int SPI_HALServer::get_int_fds(int subsystem, struct pollfd * poll_fds) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	memset((void*)poll_fds, 0, sizeof(struct pollfd));
	int index;
	switch (subsystem)  {
		case HARDWARE_LOCATION_EPS:
			index = 0;
			break;
		case HARDWARE_LOCATION_COM:
			index = 1;
			break;
		case HARDWARE_LOCATION_ACS:
			index = 2;
			break;
		case HARDWARE_LOCATION_PLD:
			index = 3;
			break;
		default:
			logger->Warning("get_int_fds(): index out of range!");
			return -1;
	}
	poll_fds->fd = int_fds[index];


	poll_fds->events = POLLPRI;
	return 0;
}

int SPI_HALServer::get_slave_fd(int subsystem) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int index;
	switch (subsystem) {
		case HARDWARE_LOCATION_EPS:
			index = 0;
			break;
		case HARDWARE_LOCATION_COM:
			index = 1;
			break;
		case HARDWARE_LOCATION_ACS:
			index = 2;
			break;
		case HARDWARE_LOCATION_PLD:
			index = 3;
			break;
		default:
			logger->Warning("get_slave_fd(): index out of range!");
			return -1;
	}
	return spi_fds[index];
}

void SPI_HALServer::logTXSuccess(int dest, uint8 opcode) {
	switch (dest) {
	case HARDWARE_LOCATION_EPS:
		TLM_SPI_EPS_TX(opcode);
		break;
	case HARDWARE_LOCATION_COM:
		TLM_SPI_EPS_TX(opcode);
		break;
	case HARDWARE_LOCATION_ACS:
		TLM_SPI_EPS_TX(opcode);
		break;
	case HARDWARE_LOCATION_PLD:
		TLM_SPI_EPS_TX(opcode);
		break;
	default:
		break;
	}
}

void SPI_HALServer::logTXFailure(int dest, uint8 opcode) {
	switch (dest) {
	case HARDWARE_LOCATION_EPS:
		TLM_SPI_EPS_TX_FAIL(opcode);
		break;
	case HARDWARE_LOCATION_COM:
		TLM_SPI_EPS_TX_FAIL(opcode);
		break;
	case HARDWARE_LOCATION_ACS:
		TLM_SPI_EPS_TX_FAIL(opcode);
		break;
	case HARDWARE_LOCATION_PLD:
		TLM_SPI_EPS_TX_FAIL(opcode);
		break;
	default:
		break;
	}
}

void SPI_HALServer::set_packet_sourcedest(int index, ACPPacket * packet) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	LocationIDType source;
	LocationIDType dest;

	// set based on index from RX loop that corresponds to the fd
	switch (index) {
	case 0:
		logger->Info("--------------------------------------------------------- SPI RX Packet from " "\x1b[34m" "EPS" "\x1b[0m" " received");
		TLM_SPI_EPS_RX(packet->getOpcode());
		source = HARDWARE_LOCATION_EPS;
		dest = SERVER_LOCATION_EPS;
		break;
	case 1:
		logger->Info("--------------------------------------------------------- SPI RX Packet from " "\x1b[34m" "COM" "\x1b[0m" " received");
		TLM_SPI_COM_RX(packet->getOpcode());
		source = HARDWARE_LOCATION_COM;
		dest = SERVER_LOCATION_COM;
		break;
	case 2:
		logger->Info("--------------------------------------------------------- SPI RX Packet from " "\x1b[34m" "ACS" "\x1b[0m" " received");
		TLM_SPI_ACS_RX(packet->getOpcode());
		source = HARDWARE_LOCATION_ACS;
		dest = SERVER_LOCATION_ACS;
		break;
	case 3:
		logger->Info("--------------------------------------------------------- SPI RX Packet from " "\x1b[34m" "PLD" "\x1b[0m" " received");
		TLM_SPI_PLD_RX(packet->getOpcode());
		source = HARDWARE_LOCATION_PLD;
		dest = SERVER_LOCATION_PLD;
		break;
	default:
		logger->Warning("set_packet_dest(): index out of range!");
		source = LOCATION_ID_INVALID;
		dest = LOCATION_ID_INVALID;
		break;
	}
	packet->setSource(source);
	packet->setDestination(dest);
}

void SPI_HALServer::GPIOsetup(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char spi_devices[NUM_SLAVES][STR_LEN];
	char int_val[NUM_SLAVES][STR_LEN];

	int i ,ret;

	uint8_t mode = SPI_MODE_0;
	uint32_t speed = 1000000;
	uint8_t clearBuf;
	memset((void*)poll_fds, 0, sizeof(poll_fds));

	//Initalize GPIO INT Pins TODO EXPORT PINS AND SET INTS
	logger->Debug("SPI_Server: Loop(): Initializing GPIO INT pins");
	bool gpioCheck = true;

	gpioCheck &= (-1 != system("echo \"134\" > /sys/class/gpio/export"));
	gpioCheck &= (-1 != system("echo \"in\" > /sys/class/gpio/pioE6/direction"));
	gpioCheck &= (-1 != system("echo \"falling\" > /sys/class/gpio/pioE6/edge"));

	gpioCheck &= (-1 != system("echo \"102\" > /sys/class/gpio/export"));
	gpioCheck &= (-1 != system("echo \"in\" > /sys/class/gpio/pioD6/direction"));
	gpioCheck &= (-1 != system("echo \"falling\" > /sys/class/gpio/pioD6/edge"));

	gpioCheck &= (-1 != system("echo \"4\" > /sys/class/gpio/export"));
	gpioCheck &= (-1 != system("echo \"in\" > /sys/class/gpio/pioA4/direction"));
	gpioCheck &= (-1 != system("echo \"falling\" > /sys/class/gpio/pioA4/edge"));

	gpioCheck &= (-1 != system("echo \"25\" > /sys/class/gpio/export"));
	gpioCheck &= (-1 != system("echo \"in\" > /sys/class/gpio/pioA25/direction"));
	gpioCheck &= (-1 != system("echo \"falling\" > /sys/class/gpio/pioA25/edge"));

	if (!gpioCheck) {
		// TODO: figure out if this needs handling
		logger->Warning("SPI_Server: Loop(): Bad return on GPIO pin");
	}

	//Open spi device File Descriptors
	strcpy(&spi_devices[0][0],"/dev/spidev32765.0");
	strcpy(&spi_devices[1][0],"/dev/spidev32765.1");
	strcpy(&spi_devices[2][0],"/dev/spidev32765.2");
	strcpy(&spi_devices[3][0],"/dev/spidev32765.3");

	//Open INT gpio File Descriptors
	strcpy(&int_val[0][0],"/sys/class/gpio/pioE6/value");
	strcpy(&int_val[1][0],"/sys/class/gpio/pioD6/value");
	strcpy(&int_val[2][0],"/sys/class/gpio/pioA4/value");
	strcpy(&int_val[3][0],"/sys/class/gpio/pioA25/value");

	//FD_ZERO(&int_fd_set);

	logger->Debug("SPI_Server: Loop(): Opening file descriptors");

	for (i = 0; i < NUM_SLAVES; i++) {
		spi_fds[i] = open(spi_devices[i], O_RDWR);
		if (spi_fds[i] < 0) {
			printf("%s\n", spi_devices[i]);
			printf("Error opening spi fd %d\n", i);
		}
		ret = ioctl(spi_fds[i], SPI_IOC_WR_MODE, &mode);
		if (ret == -1) {
			perror("Failed to set write mode");
		}
		ret = ioctl(spi_fds[i], SPI_IOC_RD_MODE, &mode);
		if (ret == -1) {
			perror("Failed to set read mode");
		}
		ret = ioctl(spi_fds[i],SPI_IOC_WR_MAX_SPEED_HZ, &speed);
		if (ret == -1) {
			perror("Failed to set write max speed");
		}
		ret = ioctl(spi_fds[i],SPI_IOC_RD_MAX_SPEED_HZ, &speed);
		if (ret == -1) {
			perror("Failed to set read max speed");
		}
		int_fds[i] = open(int_val[i], O_RDONLY);
		poll_fds[i].fd = int_fds[i];
		//FD_SET(int_fds[i], &int_fd_set);
		if (int_fds[i] < 0) {
			printf("Error opening int fd %d\n", i);
			perror("error =");
		}
		poll_fds[i].events = POLLPRI;
		read(int_fds[i], &clearBuf, 1);
	}
}
