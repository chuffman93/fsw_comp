/*
 * ETHERNET_Server.h
 *
 *  Created on: Jun 26, 2015
 *      Author: Umang
 */
#ifndef SPI_SERVER_H_
#define SPI_SERVER_H_

#include "core/Singleton.h"
#include "core/Dispatcher.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <poll.h>

using namespace std;


#define        QLEN        32    /* maximum connection queue length    */
#define        BUFSIZE        512
#define     CMDLEN        BUFSIZE
#define     server_name_len    8
#define     N_SERVERS    10

#define NUM_SLAVES 3

#define TXD_PACKET_SIG		SIGUSR1
#define RXD_PACKET_SIG		SIGUSR2

void receivedComplete(int signum);
void sendComplete(int signum);


class SPI_HALServer: public Phoenix::Core::Singleton
{
	public:
		struct pollfd poll_fds;
		int spi_fds[NUM_SLAVES];
		int int_fds[NUM_SLAVES];

		static char * queueNameSPITX;

		// Constructor
		SPI_HALServer();

		void SPI_HALServerLoop(void);

//		void spi_reset(void);

		int SPIDispatch(Phoenix::Core::FSWPacket & packet);

//		static void spi_rx_handler(int signum);
//
//		static void spi_tx_complete(int signum);

		int spi_write(int slave_fd, struct pollfd * fds, uint8_t* buf, int len);

		int spi_read(int slave_fd, struct pollfd * fds, uint8 **rx_buf);

		int get_int_fds(int subsystem, struct pollfd * poll_fds);

		int get_slave_fd(int subsystem);

	private:
		// SPI Dispatch Queue
		mqd_t queueHandleTX;
		struct mq_attr queueAttrTX;
		int qInitTX;
};

//static bool SPI_HALServer::packetWaiting;

#endif /* ETHERNET_SERVER_H_ */
