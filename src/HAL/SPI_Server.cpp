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

SPI_HALServer::SPI_HALServer()
	{
		// Intentionally left blank
	}

// Enter this loop
void SPI_HALServer::SPI_HALServerLoop(void)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char spi_devices[NUM_SLAVES][STR_LEN];
	char int_val[NUM_SLAVES][STR_LEN];
	fd_set int_fd_set;
	struct pollfd poll_fds[NUM_SLAVES];

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



	while (1) {

		printf("\n*****************************************************\n");
		printf("SPI_HAL Server: Waiting for messages\n ");
		printf("\n*****************************************************\n");
//		char buf[10] = "Start";
//		system("echo \"Start\" > Logfile.txt");

		errno = 0;

		//TODO SPI Read Loop
		//Wait for interrupt
		printf("Waiting for interrupt\n");

		usleep(1000000000);

		//Poll for slave start on INT pins
		//int retval = poll(poll_fds, NUM_SLAVES, -1);
		//printf("retval = %d, revents = %x\n", retval, poll_fds[0].revents);

		//TODO Correct take lock
//		if (true == this->TakeLock(20)){
//
//			//Figure out if there was an interrupt
//			for(int i = 0; i < NUM_SLAVES; i++){
//				puts("reading interrupt fds...\n");
//				read(int_fds[i], &buf, 1);
//				if(poll_fds[i].revents & POLLPRI){
//					//send null packet
//					packet = new FSWPacket(SERVER_LOCATION_MIN + i, HARDWARE_LOCATION_MIN + i , 1, 0, NULL);
//					printf("found interrupt on fds %d\n", i);
//					SPIDispatch(*packet);
//					break;
//				}
//			}
//			this->GiveLock();
//		}
		printf("\n*****************************************************\n ");

	}
}

int SPI_HALServer::SPIDispatch(Phoenix::Core::FSWPacket & packet){
	int bytes_copied;
	int nbytes;
	int ret;
	int slave_fd;
	uint8 * rx_buf;
	ssize_t packetLength;
	uint8_t * packetBuffer;
	struct pollfd fds;
	FSWPacket * retPacket;
	int destination = 0;
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	//char buff[6] = "Valid";

	packetLength = packet.GetFlattenSize();

	char * out = new char[100];
	snprintf(out, 99, "SPI_Server: SPIDispatch(): Hardware dispatch packet size %d", packetLength);
	logger->Log(out, LOGGER_LEVEL_DEBUG);
	delete out;

	if(packetLength >= MAX_PACKET_SIZE)
	{
		//packet is too large
		return -1;
	}

	packetBuffer = (uint8_t *) malloc(packetLength);
	//check if whole packet was copied
	ret = packet.Flatten(packetBuffer,packetLength);
	if (ret != packetLength)
	{
		//failed to flatten packet
		logger->Log("SPI_Server: SPIDispatch(): Packet flatten fail", LOGGER_LEVEL_WARN);
		return -2;
	}

	destination = packet.GetDestination();
	int source = packet.GetSource();
	printf("destination = %d\n", destination);
	printf("source = %d\n", source);
	get_int_fds(destination-1, &fds);
	slave_fd = get_slave_fd(destination);

	//take_lock
	logger->Log("SPI_Server: SPIDispatch(): Writing packet to SPI", LOGGER_LEVEL_DEBUG);
	bytes_copied = this->spi_write(slave_fd, &fds, packetBuffer, packetLength);
	logger->Log("SPI_Server: SPIDispatch(): Waiting on return message", LOGGER_LEVEL_DEBUG);
	//give lock
	nbytes = spi_read(slave_fd, &fds, &rx_buf);
	logger->Log("SPI_Server: SPIDispatch(): Received return message!", LOGGER_LEVEL_DEBUG);
	printf("packet = %2X\n", rx_buf);

	retPacket = new FSWPacket(rx_buf, nbytes);
	logger->Log("Now putting that FSW Packet into the message queue using Dispatch!", LOGGER_LEVEL_DEBUG);

	if((retPacket->GetDestination() == LOCATION_ID_INVALID )|| (retPacket->GetSource() == LOCATION_ID_INVALID))
	{
		logger->Log("FSW Packet src or dest invalid. Not placing on queue\n", LOGGER_LEVEL_DEBUG);
		printf("src %d dest %d\n", retPacket->GetDestination(), retPacket->GetSource());
		//todo log error
		delete retPacket;
	}
	else{

		Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if(!dispatcher->Dispatch(*retPacket))
		{
			printf("\r\nError in dispatch\r\n");
		}
		printf("SPI Server: Dispatched packet successfully\n");


		system("echo \"Yay\" > Logfile.txt");

		delete retPacket;
	}

	return bytes_copied;
}

int SPI_HALServer::spi_write(int slave_fd, struct pollfd * fds, uint8_t* buf, int len){
	int buf_pt = 0;
	int wr_size = 1;
	int timeout = -1;
	int ret;
	uint8_t dummy;
	//char dummy[64];
	int fd = fds->fd;
	//TODO Add timeout
	read(fds->fd, &dummy, 1);

	while(buf_pt != len){
		printf("Writing byte %d\n", buf_pt);
		printf("Byte value: %x\n", *(buf + buf_pt));
		ret = write(slave_fd, buf + buf_pt, wr_size);
		if(ret != wr_size){
			perror("Error sending byte\n");
		}
		buf_pt++;

		//Wait for interrupt before sending next byte
		printf("Waiting for interrupt HERE\n");
		if (true == this->TakeLock(2000)){
			printf("Polling\n");
			int retval = poll(fds, 1, timeout);
			printf("Poll return: %d", retval);
			this->GiveLock();
		}
		printf("After poll\n");
		ret = read(fds->fd, &dummy, 1);
		printf("revent = 0x%x\n", fds->revents);
		if(fds->fd < 0){
			perror("Err opening\n");
		}
		//printf("Read %d bytes\n", ret);
	}
	return buf_pt;
}

int SPI_HALServer::spi_read(int slave_fd, struct pollfd * fds, uint8 **rx_bufp){

	int buf_pt = 0;
	int wr_size = 1;
	int timeout = -10;
	int ret, i;
	uint8_t dummy;
	//char dummy[64];
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
			printf("Waiting for interrupt\n");
			poll(fds, 1, timeout);
			ret = read(fds->fd, &dummy, 1);
			printf("Reading byte %d: ", buf_pt);
			tr.tx_buf =  (unsigned long) &tx;
			tr.rx_buf =  (unsigned long) &rx;

			ret = ioctl(slave_fd, SPI_IOC_MESSAGE(1), &tr);
			rx_buf[buf_pt] = rx;

			printf("0x%X\n", rx);
			buf_pt++;
		}

		printf("Allocating memory for packet\n");
		//Allocate more memory based on length
		packet_len = (rx_buf[12] << 8 | rx_buf[13]) + 16;
		printf("allocating %d\n", packet_len);
		rx_buf = (uint8 *) realloc(rx_buf, packet_len * sizeof(uint8));

		//Read in remaining bytes
		for(i = 0; i < packet_len - 14; i++){
			//Wait for interrupt before sending next byte
			printf("Waiting for interrupt\n");
			poll(fds, 1, timeout);
			ret = read(fds->fd, &dummy, 1);
			printf("Reading byte %d: ", buf_pt);
			tr.tx_buf =  (unsigned long) &tx;
			tr.rx_buf =  (unsigned long) &rx;

			ret = ioctl(slave_fd, SPI_IOC_MESSAGE(1), &tr);
			rx_buf[buf_pt] = rx;

			printf("0x%x\n", rx);
			buf_pt++;
		}

		fds->fd = fd;
		fds->events = POLLPRI;
		poll(fds, 1, timeout);
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

