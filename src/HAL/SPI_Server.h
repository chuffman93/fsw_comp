/*
 * ETHERNET_Server.h
 *
 *  Created on: Jun 26, 2015
 *      Author: Umang
 */
#ifndef ETHERNET_SERVER_H_
#define ETHERNET_SERVER_H_

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


#define        BUFSIZE        512
#define     CMDLEN        BUFSIZE


class SPI_HALServer: public Phoenix::Core::Singleton
{
	public:
	int spiFileDescriptor;

    // The Host names are indexed by their HardwareLocationIDType
    char host[HARDWARE_LOCATION_MAX][INET_ADDRSTRLEN]; // Stores the IP Address of the different servers in the char array format of size INET_ADDRSTRLEN each


        // Constructor
        SPI_HALServer()
        {
        	spiFileDescriptor = 0;
        	const char * spiFilePath = "/dev/phoenix_spi";
        	// HARDWARE INIT: Open up the /dev/phoenix_spi file and init signals
        	//spiFileDescriptor = open(spiFiePath, O_RDWR);

        	/*if(signal(SENT_COMPLETE_SIG,sendComplete) == SIG_ERR)
        	{
        		//error
        	}
        	if(signal(RECEIVED_PACKET_SIG,receivedComplete) == SIG_ERR)
        	{
        		//error

        	}*/
        	if(spiFileDescriptor < 0)
        	{
        		printf("SPI_HALServer: Failed to initialize SPI driver\n");
        		//todo error logging
            }
            printf("SPI_HALServer: Initialized SPI driver\n");
            //send_sock = UDPsock("10.14.134.204",port_num_client);
        }

        /*------------------------------------------------------------------------
         * UDPsock - allocate & bind for recieving data on a socket using UDP on local host
         *------------------------------------------------------------------------
         */
        void    SPIReset(void);
        /*------------------------------------------------------------------------
         * UDPsock - allocate  a socket using UDP for sending. This will also populate the UDP fsin
         *------------------------------------------------------------------------
         */
        void receivedComplete(int signum);


        void SPI_HALServerLoop(void);

        /*------------------------------------------------------------------------
         * TCPsock - allocate & bind a server socket using TCP
         *------------------------------------------------------------------------
         */
};




#endif /* ETHERNET_SERVER_H_ */
