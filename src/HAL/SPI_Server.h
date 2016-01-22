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

#define PORT_N_SERVER "5004"         // Default socket used for server communication
#define PORT_N_CLIENT "5002"         // Default socket used for client communication

struct SessionInfo {
    char name[server_name_len];    /*Name of the session Server*/
    struct sockaddr_in fsin;    /* TCP addresses of the clients    */
    pid_t s_pid;                /*Process id of the session*/
    bool Initd;                    /*If true, means that the session exists*/
};


class SPI_HALServer: public Phoenix::Core::Singleton
{
	public:
	int spiFileDescriptor;


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



        void SPI_HALServerLoop(void);

        void spiReset(void);
        void sendComplete(int signum);
        void receivedComplete(int signum);


};


#endif /* ETHERNET_SERVER_H_ */
