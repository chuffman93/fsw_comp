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
#include "core/ACPPacket.h"


using namespace std;


#define        QLEN        32    /* maximum connection queue length    */
#define        BUFSIZE        512
#define     CMDLEN        BUFSIZE
#define     server_name_len    8
#define     N_SERVERS    10

#define PORT_N_SERVER "5004"         // Default socket used for server communication
#define PORT_N_CLIENT "5002"         // Default socket used for client communication

struct SessionInfo {
    char name[server_name_len];		/*Name of the session Server*/
    struct sockaddr_in fsin;		/* TCP addresses of the clients    */
    pid_t s_pid;					/*Process id of the session*/
    bool Initd;						/*If true, means that the session exists*/
};


class ETH_HALServer: public AllStar::Core::Singleton
{
	public:
		char port_num_server[6];							// Storing the socket number in a char array
		char port_num_client[HARDWARE_LOCATION_MAX][5];		// Storing the socket number in a char array
		int recv_sock;										// master server socket
		int send_sock[HARDWARE_LOCATION_MAX];				// Client send socket
		struct sockaddr_in fsin[HARDWARE_LOCATION_MAX];		// TCP addresses of the HARDWARE LOCATION SUBSYSTEMS


		// The Host names are indexed by their HardwareLocationIDType
		char host[HARDWARE_LOCATION_MAX][INET_ADDRSTRLEN]; // Stores the IP Address of the different servers in the char array format of size INET_ADDRSTRLEN each

        // Constructor
        ETH_HALServer()
        {
        	printf("\r\nStartining initialization of ETHERNET SERVER\r\n");
            // Initialize the server port used for recieving
            strcpy(port_num_server,"5004");


            // Initializing the ports corresponding to the hardware server
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN][0]),"5001");
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN+1][0]),"5002");
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN+2][0]),"5000");
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN+3][0]),"5005");
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN+4][0]),"5006");
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN+5][0]),"5007");
            strcpy(&(port_num_client[HARDWARE_LOCATION_MIN+6][0]),"5008");


            // Initialize the host IP Addresses
            strcpy(&(host[HARDWARE_LOCATION_MIN][0]),"10.14.134.250");
            strcpy(&(host[HARDWARE_LOCATION_MIN+1][0]),"10.14.134.252");
            strcpy(&(host[HARDWARE_LOCATION_MIN+2][0]),"10.14.134.203");
            strcpy(&(host[HARDWARE_LOCATION_MIN+3][0]),"10.14.134.204");
            strcpy(&(host[HARDWARE_LOCATION_MIN+4][0]),"10.14.134.205");
            strcpy(&(host[HARDWARE_LOCATION_MIN+5][0]),"10.14.134.205");
            strcpy(&(host[HARDWARE_LOCATION_MIN+6][0]),"10.14.134.206");


            printf("\r\nCreating sockets for receiving at socket %s",port_num_server);
            // Creating the UDP port for receiving packets from the servers. Always intialize this before intializing the sending sockets
            recv_sock = UDPsock(port_num_server);

            // Now Initializing the UDP sending sockets to different servers
            printf("\r\nCreating sockets for sending\r\n");
            for (int i=HARDWARE_LOCATION_MIN;i<HARDWARE_LOCATION_MIN+4;i++)
            {
                send_sock[i] = UDPsock(host[i],port_num_client[i],&(fsin[i]));
            }
            printf("\r\nUDP ports initialized\r\n");
            //send_sock = UDPsock("10.14.134.204",port_num_client);
        }

        int ETH_GetSendSocket(LocationIDType HardwareLocation)
        {
            return send_sock[HardwareLocation];
        }

        struct sockaddr_in* ETH_GetSendFSin(LocationIDType HardwareLocation)
        {
            return &(fsin[HardwareLocation]);
        }

        /*------------------------------------------------------------------------
         * UDPsock - allocate & bind for recieving data on a socket using UDP on local host
         *------------------------------------------------------------------------
         */
        int    UDPsock(const char *portnum);
        /*------------------------------------------------------------------------
         * UDPsock - allocate  a socket using UDP for sending. This will also populate the UDP fsin
         *------------------------------------------------------------------------
         */
        int UDPsock(const char *host, const char *portnum, struct sockaddr_in *fsin1);
        /*
         * Arguments:
         *      host      - name of host to which connection is desired
         *      portnum   - server port number
         */



        void ETH_HALServerLoop(void);

        /*------------------------------------------------------------------------
         * TCPsock - allocate & bind a server socket using TCP
         *------------------------------------------------------------------------
         */
        int     TCPsock(struct sockaddr_in *fsin,char *portnum, int qlen);

        int ETHDispatch(AllStar::Core::ACPPacket & packet);


};




int        errexit(const char *format, ...);

int echo(int fd);



#endif /* ETHERNET_SERVER_H_ */
