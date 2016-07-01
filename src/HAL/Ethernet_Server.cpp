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
#include <pthread.h>

#include "Ethernet_Server.h"
#include "core/Dispatcher.h"
#include "util/Logger.h"


struct SessionInfo SS[N_SERVERS];


using namespace Phoenix;
using namespace Core;

// Enter this loop
void ETH_HALServer::ETH_HALServerLoop(void)
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	fd_set	rfds;				/* read file descriptor set		*/
	fd_set 	wfds;				/* write file descriptor set	*/
	fd_set	afds;				/* active file descriptor set	*/
	unsigned int alen;			/* from-address length			*/
	int	fd, nfds;
	int numbytes;
	char args[BUFSIZE];
	char *argsp;
	char cmd[CMDLEN];
	char *cmdp;


	// Declarations for FSW packet to Message queue
	int packetSize;
	uint8_t * buffer;
	FSWPacket * packet;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(this->recv_sock, &afds);

	pthread_t self_id;
	self_id = pthread_self();
	printf("ETHHALServerLoop: Thread %u\n", self_id);

	while (1) {

		memcpy(&rfds, &afds, sizeof(rfds));

		/* Makes the connections wait for the filedescriptor to be set*/
		logger->Log("*****************************************************", LOGGER_LEVEL_INFO);
		logger->Log("ETH_HAL Server: Waiting for messages", LOGGER_LEVEL_INFO);
		logger->Log("*****************************************************", LOGGER_LEVEL_INFO);
		/*Waiting for a file descriptor to have its connection set*/
		if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0,
				(struct timeval *)0) < 0)
			errexit("select: %s\n", strerror(errno));
		if (FD_ISSET(this->recv_sock, &rfds)) {

		alen = sizeof(fsin);

		logger->Log("ETH_HAL Server: Recieving data", LOGGER_LEVEL_DEBUG);
		if ((numbytes = recvfrom(this->recv_sock, cmd, sizeof(cmd) , 0, (struct sockaddr *)&fsin, &alen)) == -1)
		{
			perror("recvfrom");
			exit(1);
		}
		printf("listener: packet contains \"%s\ bytes %d \n", cmd, numbytes);

//		Command_Parser(cmd,args);
		FD_SET(this->recv_sock, &afds);
//		FD_SET(pipe1[0],&afds);


		buffer = (uint8_t*) malloc(numbytes);
		memcpy((uint8_t *) buffer, cmd,numbytes);
		logger->Log("ETH_HAL Server: Converting a Pheonix Packet into a FSW packet", LOGGER_LEVEL_DEBUG);

		packet =new FSWPacket(buffer, numbytes);
		logger->Log("ETH_HAL Server: Now putting that FSW Packet into the message queue using Dispatch!", LOGGER_LEVEL_DEBUG);

		//packet->SetDestination(SERVER_LOCATION_ACS);

		if((packet->GetDestination() == LOCATION_ID_INVALID )|| (packet->GetSource() == LOCATION_ID_INVALID))
		{
			logger->Log("ETH_HAL Server: FSW Packet src or dest invalid. Not placing on queue", LOGGER_LEVEL_DEBUG);
			printf("src %d dest %d\n", packet->GetDestination(), packet->GetSource());
			//todo log error
			delete packet;
			free(buffer);
		}
		else{
			if(!dispatcher->Dispatch(*packet))
			{
				// TODO: handle this?
			}

			logger->Log("ETH_HAL Server: Dispatched packet successfully", LOGGER_LEVEL_INFO);

			delete packet;
			free(buffer);
		}


//		CC_Commands(cmd,args,&msock,&fsin,&alen);

		}
//		if (FD_ISSET(pipe1[0],&rfds))
//		{
//			memset(cmd,'\0',CMDLEN+1);
//			printf("There is something to be read from the pipe\n");
//			fflush(stdout);
//			numbytes=read(pipe1[0],cmd,sizeof(cmd));
//			printf("Piped Command %s of %d bytes\n",cmd,numbytes);
//			fflush(stdout);
//			Command_Parser(cmd,args);
//			CC_Commands(cmd,args,&msock,&fsin,&alen);
//		}
//		else
//		{
			// Remove any other socket read
			for (fd=0; fd<nfds; ++fd)
			if (fd != this->recv_sock && FD_ISSET(fd, &rfds))
				if (echo(fd) == 0) {
					(void) close(fd);
					FD_CLR(fd, &afds);
				}
//		}

	}

}





/*------------------------------------------------------------------------
 * UDPsock - allocate & bind a server socket using UDP
 *------------------------------------------------------------------------
 */
int ETH_HALServer::UDPsock(const char *portnum)
/*
 * Arguments:
 *      portnum   - port number of the server
 *      qlen      - maximum server request queue length
 */
{
		struct hostent *phe;
		struct sockaddr_in sin; /* an Internet endpoint address  */
		int     s;              /* socket descriptor             */
		char *host="localhost";


		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = INADDR_ANY;

	/* Map port number (char string) to port number (int) */
		if ((sin.sin_port=htons((unsigned short)atoi(portnum))) == 0)
				errexit("can't get \"%s\" port number\n", portnum);

	/* Allocate a socket */
		s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (s < 0)
			errexit("can't create socket: %s\n", strerror(errno));

	/* Bind the socket */
		if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			printf("can't bind to %s port: %s; Trying other port\n", portnum, strerror(errno));
		}

		if ( phe = gethostbyname(host) )
				printf("Server Port Address is %s:%d\n",inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
		else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
			 errexit("can't get \"%s\" host entry\n", host);

		return s;
}




/*------------------------------------------------------------------------
 * UDPsock - allocate  a socket using UDP for sending. This will also populate the UDP fsin1 argument with the correct
 *------------------------------------------------------------------------
 */
int ETH_HALServer::UDPsock(const char *host, const char *portnum, struct sockaddr_in *fsin1)
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      portnum   - server port number
 */
{
	struct hostent  *phe;   /* pointer to host information entry    */
	int     s;              /* socket descriptor                    */
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	/* Map port number (char string) to port number (int)*/
	if ((sin.sin_port=htons((unsigned short)atoi(portnum))) == 0)
			errexit("can't get \"%s\" port number\n", portnum);


	/* Map host name to IP address, allowing for dotted decimal */
	if ( phe = gethostbyname(host) )
			memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
			errexit("can't get \"%s\" host entry\n", host);


	/* Allocate a socket */
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0)
			errexit("can't create socket: %s\n", strerror(errno));

	memcpy(fsin1,&sin,sizeof(sin));
	return s;
}




/*------------------------------------------------------------------------
 * TCPsock - allocate & bind a server socket using TCP
 *------------------------------------------------------------------------
 */
int ETH_HALServer::TCPsock(struct sockaddr_in *fsin,char *portnum, int qlen)
/*
 * Arguments:
 *      portnum   - port number of the server
 *      qlen      - maximum server request queue length
 */
{
		struct sockaddr_in sin; /* an Internet endpoint address  */
		int     s;              /* socket descriptor             */

		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = INADDR_ANY;

		/* Allocate a socket */
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s < 0)
			errexit("can't create socket: %s\n", strerror(errno));


		sin.sin_port=htons(0); /* request a port number to be allocated
							   by bind */

		/* Bind the socket */
		if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
			errexit("can't bind: %s\n", strerror(errno));
		else {
			int socklen = sizeof(sin);

			if (getsockname(s, (struct sockaddr *)&sin,(unsigned int * )&socklen) < 0)
					errexit("getsockname: %s\n", strerror(errno));
			printf("New server port number is %d\n", ntohs(sin.sin_port));
			sprintf(portnum,"%d",ntohs(sin.sin_port));
		}

		if (listen(s, qlen) < 0)
			errexit("can't listen on %s port: %s\n", portnum, strerror(errno));

		memcpy(fsin,&sin,sizeof(sin));
		return s;
}

int ETH_HALServer::ETHDispatch(FSWPacket & packet)
{
	struct sockaddr_in fsin;    /* the from address of a client    */
	int alen= sizeof(fsin);
	int nbytes;
	ssize_t packetLength;
	uint8_t * packetBuffer;

	pthread_t self_id;
	self_id = pthread_self();
	printf("ETHDispatch: Thread %u\n", self_id);

	packetLength = packet.GetFlattenSize();
	printf("Hardware dispatch packet size %d",packet.GetFlattenSize());
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
		printf("failed at 2\n");
		return -2;
	}

	memcpy(&fsin,ETH_GetSendFSin(packet.GetDestination()),sizeof(*(ETH_GetSendFSin(packet.GetDestination()))));
	alen= sizeof(fsin);
	// Code for sending the Pheonix Packet onto the UDP server
	nbytes= sendto(ETH_GetSendSocket(packet.GetDestination()), packetBuffer, packetLength,0,(struct sockaddr *) &fsin,alen);
	printf("\r\nDispatched to UDP Hardware %d bytes\r\n",nbytes);

	printf("\r\nDispatched to Hardware %d bytes to server number %d\r\n",nbytes,packet.GetDestination());
	printf("\r\n Dispatching to IP Address %s and port number %s",host[packet.GetDestination()],port_num_client[packet.GetDestination()]);
	return 0;
}

/*------------------------------------------------------------------------
 * errexit - print an error message and exit
 *------------------------------------------------------------------------
 */
int errexit(const char *format, ...)
{
        va_list args;

        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        exit(1);

}


/*------------------------------------------------------------------------
 * echo - echo one buffer of data, returning byte count
 *------------------------------------------------------------------------
 */
int echo(int fd)
{
	char	buf[BUFSIZ];
	int	cc;

	cc = read(fd, buf, sizeof buf);
	if (cc < 0)
		errexit("echo read: %s\n", strerror(errno));
	if (cc && write(fd, buf, cc) < 0)
		errexit("echo write: %s\n", strerror(errno));
	return cc;

}
