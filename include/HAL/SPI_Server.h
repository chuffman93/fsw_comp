/*
 * ETHERNET_Server.h
 *
 *  Created on: Jun 26, 2015
 *      Author: Umang
 */
#ifndef SPI_SERVER_H_
#define SPI_SERVER_H_

#include "core/Singleton.h"
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


using namespace std;


#define        QLEN        32    /* maximum connection queue length    */
#define        BUFSIZE        512
#define     CMDLEN        BUFSIZE
#define     server_name_len    8
#define     N_SERVERS    10

#define TXD_PACKET_SIG		SIGUSR1
#define RXD_PACKET_SIG		SIGUSR2

void receivedComplete(int signum);
void sendComplete(int signum);


class SPI_HALServer: public Phoenix::Core::Singleton
{
	public:
	int spiFileDescriptor;
	//static bool packetWaiting;

	// Constructor
        SPI_HALServer()
        {
        	spiFileDescriptor = 0;
        	const char * spiFilePath = "/dev/phoenix_spi";
        	// HARDWARE INIT: Open up the /dev/phoenix_spi file and init signals
        	spiFileDescriptor = open(spiFilePath, O_RDWR);

        	//packetWaiting = false;

        	if(spiFileDescriptor < 0)
        	{
        		printf("SPI_HALServer: Failed to initialize SPI driver\n");
        		//todo error logging
            }
            printf("SPI_HALServer: Initialized SPI driver\n");
        }

        void SPI_HALServerLoop(void);

        void spi_reset(void);

        int SPIDispatch(Phoenix::Core::FSWPacket & packet);

        static void spi_rx_handler(int signum);

        static void spi_tx_complete(int signum);
};

//static bool SPI_HALServer::packetWaiting;

#endif /* ETHERNET_SERVER_H_ */
