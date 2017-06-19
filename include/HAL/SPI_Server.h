/*
 * SPI_Server.h
 *
 *     Created: March 2016
 *      Author: Connor Kelleher
 *     Updated: Robert Belter, Alex St. Clair
 */

#ifndef SPI_SERVER_H_
#define SPI_SERVER_H_

#include "core/Singleton.h"
#include "core/Dispatcher.h"
#include "servers/SubsystemServer.h"
#include "core/ACPPacket.h"
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
using namespace AllStar::Servers;

#define NUM_SLAVES 4

class SPI_HALServer: public AllStar::Servers::SubsystemServer, public AllStar::Core::Singleton
{
	public:
		struct pollfd poll_fds[NUM_SLAVES];
		int spi_fds[NUM_SLAVES];
		int int_fds[NUM_SLAVES];

		// SPI Dispatch Queue
		mqd_t queueHandleTX;
		struct mq_attr queueAttrTX;
		int qInitTX;
		static char * queueNameSPITX;

		// Statistics variables
		uint32 packetsDroppedTX;
		uint32 packetsSentTX;

		// Constructor
		SPI_HALServer();

		~SPI_HALServer();

		void SubsystemLoop(void);

		bool SPIDispatch(AllStar::Core::ACPPacket & packet);

	private:
		void GPIOsetup(void);

		int spi_write(int slave_fd, struct pollfd * fds, uint8_t* buf, int len);

		int spi_read(int slave_fd, struct pollfd * fds, uint8 **rx_buf);

		int get_int_fds(int subsystem, struct pollfd * poll_fds);

		int get_slave_fd(int subsystem);

		void logTXSuccess(int dest, uint8 opcode);

		void logTXFailure(int dest, uint8 opcode);

		void set_packet_sourcedest(int subsystem, AllStar::Core::ACPPacket * packet);

		BEGIN_STATE_MAP
		END_STATE_MAP
};

#endif // SPI_SERVER_H_
