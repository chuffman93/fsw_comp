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


#include "SPI_Server.h"
#include "core/FSWPacket.h"
#include "core/Dispatcher.h"


using namespace Phoenix;
using namespace Core;

// Enter this loop
void SPI_HALServer::SPI_HALServerLoop(void)
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	fd_set	rfds;			/* read file descriptor set	*/
	fd_set 	wfds;			/*	write file descriptor set*/
	fd_set	afds;			/* active file descriptor set	*/
	unsigned int alen;		/* from-address length		*/
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

	nfds = getdtablesize();
	FD_ZERO(&afds);

	while (1) {



	printf("\n*****************************************************\n ");

	}

}





/*------------------------------------------------------------------------
 * UDPsock - allocate & bind a server socket using UDP
 *------------------------------------------------------------------------
 */
void    SPI_HALServer::SPIReset(void)
/*
 * Arguments:
 *      portnum   - port number of the server
 *      qlen      - maximum server request queue length
 */
{
	char dummy = 'c';
	write(spiFileDescriptor,&dummy,1); //write one byte that will reset the state machine & clear the buffers
}


void SPI_HALServer::receivedComplete(int signum)
{
	int packetSize;
	uint8_t * buffer;
	FSWPacket * packet;
	ReturnMessage retMsg;
	LocationIDType temp;
	uint32 time;
	bool receiving = false;

	//driver returns packet size when reading only 1 byte
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	packetSize = read(spiFileDescriptor,buffer,1);

	buffer = (uint8_t*) malloc(packetSize);
	if(read(spiFileDescriptor,buffer,packetSize) != packetSize)
	{
		cout << "Error reading packet" << endl;
		//return err? What Error?
	}

	packet =new FSWPacket(buffer, packetSize);

	if (!dispatcher->Dispatch(*packet))
	{

	}

	/*if (DISPATCHER_STATUS_OK != dispatcher->WaitForDispatchResponse(*packet, retMsg))
	{
	}

	temp = packet->GetDestination();
	packet->SetDestination(packet->GetSource());
	packet->SetSource(temp);
	packet->SetTimestamp(time);
	packet->SetMessage(&retMsg);

	if (0 != dispatcher->DispatchToHardware(*packet))
	{


	}*/

	delete packet;
	free(buffer);
	return;
}

