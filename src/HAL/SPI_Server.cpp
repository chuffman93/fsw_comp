/*
 * ETHERNET_Server.c
 *
 *  Created on: Jun 26, 2015
 *      Author: Umang
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


#include "HAL/SPI_Server.h"
#include "core/FSWPacket.h"
#include "core/Dispatcher.h"
#include "util/Logger.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define STR_LEN	   50
#define RX_BUF_LEN 300

using namespace Phoenix;
using namespace Core;

bool packetWaiting;
int debugFile;
char * SPI_HALServer::queueNameSPITX = (char *) "/queueHandleSPITX";

SPI_HALServer::SPI_HALServer()
{
	mq_unlink(queueNameSPITX);
	qInitTX = mqCreate(&queueHandleTX, &queueAttrTX, queueNameSPITX);
}

SPI_HALServer::~SPI_HALServer()
{
	mq_unlink(queueNameSPITX);
}

// Enter this loop
void SPI_HALServer::SPI_HALServerLoop(void)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char spi_devices[NUM_SLAVES][STR_LEN];
	char int_val[NUM_SLAVES][STR_LEN];
	fd_set int_fd_set;

	int i ,ret, len;

	uint8_t mode = SPI_MODE_1;
	uint32_t speed = 1000000;
	uint8_t buf;
	int timeout = -1;
	FSWPacket * packet;

	memset((void*)poll_fds, 0, sizeof(poll_fds));

	debugFile = open("~/Logfile.txt",  O_RDWR);

	//Initalize GPIO INT Pins TODO EXPORT PINS AND SET INTS
	logger->Log("SPI_Server: Loop(): Initializing GPIO INT pins", LOGGER_LEVEL_DEBUG);
	system("echo \"134\" > /sys/class/gpio/export");
	system("echo \"in\" > /sys/class/gpio/pioE6/direction");
	system("echo \"falling\" > /sys/class/gpio/pioE6/edge");
	system("echo \"102\" > /sys/class/gpio/export");
	//system("echo \"in\" > /sys/class/gpio/pioB27/direction");
	system("echo \"rising\" > /sys/class/gpio/pioD6/edge");
	system("echo \"4\" > /sys/class/gpio/export");
	//system("echo \"in\" > /sys/class/gpio/pioB28/direction");
	system("echo \"rising\" > /sys/class/gpio/pioA4/edge");

	//Open spi device File Descriptors
	strcpy(&spi_devices[0][0],"/dev/spidev32765.0");
	strcpy(&spi_devices[1][0],"/dev/spidev32765.1");
	strcpy(&spi_devices[2][0],"/dev/spidev32765.2");

	//Open INT gpio File Descriptors
	strcpy(&int_val[0][0],"/sys/class/gpio/pioE6/value");
	strcpy(&int_val[1][0],"/sys/class/gpio/pioD6/value");
	strcpy(&int_val[2][0],"/sys/class/gpio/pioA4/value");

	//FD_ZERO(&int_fd_set);

	logger->Log("SPI_Server: Loop(): Opening file descriptors", LOGGER_LEVEL_DEBUG);

	for(i = 0; i < NUM_SLAVES; i++){
		spi_fds[i] = open(spi_devices[i], O_RDWR);
		if(spi_fds[i] < 0){
			printf("%s\n", spi_devices[i]);
			printf("Error opening spi fd %d\n", i);
		}
		ioctl(spi_fds[i], SPI_IOC_WR_MODE, &mode);
		ret = ioctl(spi_fds[i],SPI_IOC_WR_MAX_SPEED_HZ, &speed);
		if(ret == -1){
			perror("Failed to set max speed");
		}
		int_fds[i] = open(int_val[i], O_RDONLY);
		poll_fds[i].fd = int_fds[i];
		//FD_SET(int_fds[i], &int_fd_set);
		if(int_fds[i] < 0){
			printf("Error opening int fd %d\n", i);
			perror("error =");
		}
		poll_fds[i].events = POLLPRI;
		read(int_fds[i], &buf, 1);
	}

	logger->Log("*****************************************************", LOGGER_LEVEL_INFO);
	logger->Log("SPI_HAL Server: Entering Loop --------------------- *", LOGGER_LEVEL_INFO);
	logger->Log("*****************************************************", LOGGER_LEVEL_INFO);

	int enterTime;
	int slave_fd;
	int nbytes;
	struct pollfd fds;
	uint8 * rxBuf;
	FSWPacket * rxPacket;
	while (1) {
		errno = 0;
		enterTime = getTimeInMilis();

		// ---- TX ----------------------------------------------------------------------------------------------------------------
		FSWPacket * txPacket;
		if(true == this->TakeLock(MAX_BLOCK_TIME)){
			if(mq_size(queueHandleTX, queueAttrTX) > 0){
				if(mq_timed_receive(queueNameSPITX, &txPacket, 0, DISPATCHER_MAX_DELAY)){
					SPIDispatch(*txPacket);
				}else{
					logger->Log("SPI_HAL Server: Queue receive for TX failed!", LOGGER_LEVEL_WARN);
				}
			}else{
				logger->Log("SPI_HAL Server: No packets for TX", LOGGER_LEVEL_DEBUG);
			}
			this->GiveLock();
		}else{
			logger->Log("SPI_HAL Server: TX semaphore failed", LOGGER_LEVEL_WARN);
		}

		// ---- RX ----------------------------------------------------------------------------------------------------------------
		poll(poll_fds, NUM_SLAVES, 10);
		if (true == this->TakeLock(MAX_BLOCK_TIME)){
			//Figure out if there was an interrupt
			for (int i = 0; i < NUM_SLAVES; i++){
				read(int_fds[i], &buf, 1);
				if (poll_fds[i].revents & POLLPRI){
					logger->Log("SPI_HAL Server: found interrupt on fds %d\n", i, LOGGER_LEVEL_DEBUG);
					slave_fd = get_slave_fd(i);
					get_int_fds(i-1, &fds);
					nbytes = spi_read(slave_fd, &fds, &rxBuf);
					rxPacket = new FSWPacket(rxBuf, nbytes);

					// Check bounds and send to dispatcher RX queue
					if ((rxPacket->GetDestination() == LOCATION_ID_INVALID )|| (rxPacket->GetSource() == LOCATION_ID_INVALID)){
						logger->Log("FSW Packet src or dest invalid. Not placing on queue", LOGGER_LEVEL_WARN);
						logger->Log("Ret dest: %d", rxPacket->GetDestination(), LOGGER_LEVEL_DEBUG);
						logger->Log("Ret source: %d", rxPacket->GetSource(), LOGGER_LEVEL_DEBUG);
						delete rxPacket;
					}else{
						Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
						while (!mq_timed_send(dispatcher->queueNameRX, &rxPacket, MAX_BLOCK_TIME, 0));
					}
				}else{
					logger->Log("SPI_HAL Server: No RX interrupts found", LOGGER_LEVEL_DEBUG);
				}
			}
			this->GiveLock();
		}else{
			logger->Log("SPI_HAL Server: RX semaphore failed", LOGGER_LEVEL_WARN);
		}

		// FIXME: decrease wait time
		waitUntil(enterTime, 500); // wait 500 ms
	}
}

int SPI_HALServer::SPIDispatch(Phoenix::Core::FSWPacket & packet){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	int bytes_copied;
	int ret;
	int slave_fd;
	ssize_t packetLength;
	uint8_t * packetBuffer;
	struct pollfd fds;
	int destination = 0;

	packetLength = packet.GetFlattenSize();
	logger->Log("SPI_Server: SPIDispatch(): Dispatching packet of size %d", (uint32) packetLength, LOGGER_LEVEL_DEBUG);

	if(packetLength >= MAX_PACKET_SIZE)
	{
		logger->Log("SPI_Server: SPIDispatch(): Packet too large!", LOGGER_LEVEL_ERROR);
		return -1;
	}

	packetBuffer = (uint8_t *) malloc(packetLength);
	ret = packet.Flatten(packetBuffer, packetLength);

	//check if whole packet was copied
	if (ret != packetLength)
	{
		logger->Log("SPI_Server: SPIDispatch(): Packet flatten fail", LOGGER_LEVEL_WARN);
		return -2;
	}

	destination = packet.GetDestination();
	int source = packet.GetSource();
	logger->Log("destination = %d\n", destination, LOGGER_LEVEL_DEBUG);
	logger->Log("source = %d\n", source, LOGGER_LEVEL_DEBUG);
	get_int_fds(destination-1, &fds);
	slave_fd = get_slave_fd(destination);

	logger->Log("SPI_Server: SPIDispatch(): Writing packet to SPI", LOGGER_LEVEL_DEBUG);
	bytes_copied = this->spi_write(slave_fd, &fds, packetBuffer, packetLength);
	logger->Log("SPI_Server: SPIDispatch(): Waiting on return message", LOGGER_LEVEL_DEBUG);

	return bytes_copied;
}

int SPI_HALServer::spi_write(int slave_fd, struct pollfd * fds, uint8_t* buf, int len){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int buf_pt = 0;
	int wr_size = 1;
	int ret;
	uint8_t dummy;
	read(fds->fd, &dummy, 1);

	while(buf_pt != len){
		logger->Log("Writing byte %d", buf_pt, LOGGER_LEVEL_DEBUG);
		logger->Log("Byte value: ----------------------- 0x%x", *(buf + buf_pt), LOGGER_LEVEL_DEBUG);
		if (true == this->TakeLock(2000)){
			ret = write(slave_fd, buf + buf_pt, wr_size);
		}else{
			logger->Log("Error writing byte, semaphore failed!", LOGGER_LEVEL_WARN);
		}
		if(ret != wr_size){
			logger->Log("spi_write: Failed to write byte!", LOGGER_LEVEL_WARN);
			perror("Error sending byte\n");
		}
		buf_pt++;

		//Wait for interrupt before sending next byte
		logger->Log("Waiting for interrupt after byte send", LOGGER_LEVEL_DEBUG);
		if (true == this->TakeLock(2000)){
			logger->Log("spi_write: Polling", LOGGER_LEVEL_DEBUG);
			int retval = poll(fds, 1, -1);
			logger->Log("spi_write: Poll return: %d", retval, LOGGER_LEVEL_DEBUG);
			this->GiveLock();
		}
		logger->Log("spi_write: After poll", LOGGER_LEVEL_DEBUG);
		ret = read(fds->fd, &dummy, 1);
		logger->Log("spi_write: revent = 0x%x", fds->revents, LOGGER_LEVEL_DEBUG);
		if(fds->fd < 0){
			perror("Err opening\n");
		}
		//printf("Read %d bytes\n", ret);
	}
	return buf_pt;
}

int SPI_HALServer::spi_read(int slave_fd, struct pollfd * fds, uint8 **rx_bufp){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int buf_pt = 0;
	int ret, i;
	uint8_t dummy;
	int fd = fds->fd;
	int rx_complete = 0;
	uint16_t packet_len = 0;

	struct spi_ioc_transfer tr;

	uint8 *rx_buf;

	tr.tx_buf = 0;
	tr.rx_buf = 0;
	tr.len = 1;
	tr.delay_usecs = 0;
	tr.speed_hz = 1000000;
	tr.cs_change = 1;
	tr.bits_per_word = 8;

	uint8 rx, tx = 0;

	rx_buf = (uint8 *) malloc(sizeof(uint8) * 16);


	//TODO Add timeout
	read(fds->fd, &dummy, 1);
	while(rx_complete != true){

		for(i = 0; i < 14; i++){
			//Wait for interrupt before sending next byte
			logger->Log("spi_read: Waiting for interrupt", LOGGER_LEVEL_DEBUG);
			poll(fds, 1, -1);
			ret = read(fds->fd, &dummy, 1);
			logger->Log("spi_read: Reading byte %d: ", buf_pt, LOGGER_LEVEL_DEBUG);
			tr.tx_buf =  (unsigned long) &tx;
			tr.rx_buf =  (unsigned long) &rx;

			ret = ioctl(slave_fd, SPI_IOC_MESSAGE(1), &tr);
			rx_buf[buf_pt] = rx;

			logger->Log("spi_read: byte value: ------------- 0x%X", rx, LOGGER_LEVEL_DEBUG);
			buf_pt++;
		}

		logger->Log("spi_read: Allocating memory for packet", LOGGER_LEVEL_DEBUG);
		//Allocate more memory based on length
		packet_len = (rx_buf[12] << 8 | rx_buf[13]) + 16;
		logger->Log("spi_read: allocating %d", packet_len, LOGGER_LEVEL_DEBUG);
		rx_buf = (uint8 *) realloc(rx_buf, packet_len * sizeof(uint8));

		//Read in remaining bytes
		for(i = 0; i < packet_len - 14; i++){
			//Wait for interrupt before sending next byte
			logger->Log("spi_read: Waiting for interrupt", LOGGER_LEVEL_DEBUG);
			poll(fds, 1, -1);
			ret = read(fds->fd, &dummy, 1);
			logger->Log("spi_read: Reading byte %d: ", buf_pt, LOGGER_LEVEL_DEBUG);
			tr.tx_buf =  (unsigned long) &tx;
			tr.rx_buf =  (unsigned long) &rx;

			ret = ioctl(slave_fd, SPI_IOC_MESSAGE(1), &tr);
			rx_buf[buf_pt] = rx;

			logger->Log("spi_read: byte value: ------------- 0x%x", rx, LOGGER_LEVEL_DEBUG);
			buf_pt++;
		}

		fds->fd = fd;
		fds->events = POLLPRI;
		poll(fds, 1, -1);
		read(fds->fd, &dummy, 1);

		*rx_bufp = rx_buf;
		return buf_pt;
	}
}

int SPI_HALServer::get_int_fds(int subsystem, struct pollfd * poll_fds){
	memset((void*)poll_fds, 0, sizeof(struct pollfd));
	poll_fds->fd = int_fds[subsystem];
	poll_fds->events = POLLPRI;
	return 0;
}

int SPI_HALServer::get_slave_fd(int subsystem){
	//subsystem = 1; // TODO: this breaks literally everything
	return spi_fds[subsystem];
}

