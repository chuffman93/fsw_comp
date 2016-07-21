/*! \file Dispatcher.h
 *  \brief Header File for the Dispatcher Class
 *
 *  This file contains the definition of the Dispatcher class, which acts
 *  as the FSWPacket-routing entity in the Phoenix architecture.
 */

#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include "core/FSWPacket.h"
#include "core/ErrorMessage.h"
#include "core/Arbitrator.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/MessageHandlerRegistry.h"
#include "core/ReturnMessage.h"
#include "core/StdTypes.h"

#include <stdint.h>
#include <map>
#include <stdio.h>

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Length of the Dispatcher Queue */
        #define DISPATCHER_QUEUE_LENGTH 		32

        /*! \brief Length of the Subsystem Queue */
        #define DISPATCHER_SUBSYS_QUEUE_LENGTH          6

        /*! \brief Dispatcher Wait Time
         *
         *  Number of kernel ticks that the Dispatcher waits between checking
         *  the queue when it is waiting for a response from a dispatched
         *  packet.
         */
        #define DISPATCHER_WAIT_TIME			(100000) //was 100 kernel ticks, 100 ms

        /*! \brief Dispatcher Maximum Delay
         *
         *  Number of kernel ticks that the Dispatcher waits when checking the
         *  queue for a packet before failing.
         */
        #define DISPATCHER_MAX_DELAY 			(10000*1000) //was 10 kernel ticks, 10 ms in ns

        /*! \brief Dispatcher Listen Maximum Delay
         *
         *  Number of kernel ticks that the Dispatcher will wait before
         *  killing a message handler and returning an error to the
         *  sending server.
         */
        #ifndef DISPATCHER_LISTEN_MAX_DELAY
        #define DISPATCHER_LISTEN_MAX_DELAY		(100000) //was 1000
        #endif // DISPATCHER_LISTEN_MAX_DELAY
		
		/*! \brief Delay Between Protocol SPI Bytes */
		#define PROTOCOL_SPI_DELAY      (200)
		
        /*! \brief Dispatcher Maximum Response Tries
         *
         *  Number of times the Dispatcher checks the queue for a packet
         *  before failing when it is waiting for a response from a
         *  dispatched packet.
         */
        #define DISPATCHER_MAX_RESPONSE_TRIES	20
		
		/*! \brief Dispatcher delay loop */
		#define	MAX_DISPATCHER_LOOP		500

    	/*! \brief Max packet size in bytes */
		#define MAX_PACKET_SIZE			65551

    	/* \brief Signals used for SPI communication*/
		#define SENT_COMPLETE_SIG		SIGUSR1
		#define RECEIVED_PACKET_SIG		SIGUSR2

        /*! \brief Class that Dispatches Packets From One Server to Another
         *
         *  This Singleton class allows one Phoenix server to send Packets
         *  to another Phoenix server.
         */
        class Dispatcher : public Singleton
        {
			/*! \brief Declare Factory a friend class 
			*	
			*	This allows factory to call Dispatcher's private constructor
			*/
			friend class Factory;
			
        public:

			typedef enum DispatcherCheckEnum{
				CHECK_MATCHING_RESPONSE,
				CHECK_DEST_SOURCE,
				CHECK_SAME
			}DispatcherCheckType;

			// Unique number for each packet so that we can track them.
			static uint16 packetNumber;

			/*! \brief Typedef for a FSWPacket Comparison Function */
			typedef bool (Dispatcher::* PacketCheckFunctionType)(const FSWPacket & packetIn, const FSWPacket & packetOut) const;

            /*! \brief Dispatcher Task for Handling Subsystem Messages
             *
             *  Waits for a subsystem to indicate that it has a message
             *  to send.  Then, the task receives the message and inserts
             *  it into the message queue.
             *
             *  \param params Parameters to the task.  Currently, this
             *  parameter can be NULL, as it is not used.
             */
            static void DispatcherTask(void * params);

            /*! \brief Adds a MessageHandlerRegistry to the Dispatcher
             *
             *  Adds registry to the Dispatcher instance and associates it
             *  with the given server ID.  Whenever a Message corresponding
             *  to that ID is received, the given registry is searched for
             *  the correct MessageHandler.
             *
             *  \param serverID Server ID to associate registry with.
             *  \param registry Pointer to the MessageHandler registry.
             *  \param arby Arbitrator to guard the permissions for the server
             *  \return true if the operation was successful and
             *  false otherwise.
             *
             *  \note The registry is added by reference.  This means that
             *  the given pointer still points to the added registry.
             *  Because the Singleton instance can never go out of scope,
             *  the user does not have to work about deleting this data and
             *  SHOULD NOT DELETE THE REGISTRY AT ANY TIME.
             *
             *  \note To ensure that the registry never goes out of scope,
             *  it should either be a global variable or allocated on the
             *  heap.
             */
            bool AddRegistry(LocationIDType serverID, MessageHandlerRegistry * registry, Arbitrator * arby);

            /*! \brief Dispatches a FSWPacket to its Destination Location
             *
             *  Dispatches the given packet to the destination contained in
             *  the packet's destination field.
             *
             *  \param packet FSWPacket to be dispatched.
             *  \return true if the operation was successful and
             *  false otherwise.
             *
             *  \note This function call should always be followed by a call
             *  to WaitForDispatchResponse.  These functions have been
             *  separated so that if both the source and destination servers
             *  are trying to Dispatch messages to each other, each will
             *  have a chance to Listen for a message before looking for
             *  the response to the Dispatched message.
             */
            bool Dispatch(FSWPacket & packet);

            /*! \brief Checks for a FSWPacket Addressed to a Given Server
             *
             *  Checks the message queue for a message for the given server.
             *  If there is a message, then the corresponding message handler
             *  is called, if it has been registered with the Dispatcher.
             *
             *  \param serverID ID of the server.
             *  \return true if there was a message and a corresponding
             *  registered handler for the message or false otherwise.
             *
             *  \note This method will always return a message to the server
             *  that sends a command to the given server.  If there is no
             *  registered handler for that message, then a standard
             *  unknown message response will be issued.
             */
            MessageHandlerRegistry * FindHandler(LocationIDType serverID, FSWPacket * packet);

            /*! \brief Dispatch a Message to Hardware
             *
             *  Sends a message to a hardware location and then receives the
             *  response and inserts it into the Dispatcher queue so that it
             *  can be handled by the appropriate server.
             *
             */

            /*! \brief Checks if a FSWPacket is a Response to Another FSWPacket
			 *
			 *  Checks if packetOut is a response to the message sent in
			 *  packetIn.
			 *
			 *  \param packetIn FSWPacket that was sent.
			 *  \param packetOut FSWPacket to check against packetIn.
			 *  \return true if packetOut is a response to packetIn and false
			 *  otherwise.
			 */
			inline bool IsPacketMatchingResponse(const FSWPacket & packetIn, const FSWPacket & packetOut) const;

			/*! \brief Checks if a FSWPacket's Destination Matches Another's Source
			 *
			 *  Checks if packetOut's destination is the same as packetIn's
			 *  source.
			 *
			 *  \param packetIn FSWPacket to check source in.
			 *  \param packetOut FSWPacket to check destination in.
			 *  \return true if packetOut's destination equals packetIn's
			 *  source and false otherwise.
			 */
			inline bool IsPacketDestMatchingSource(const FSWPacket & packetIn, const FSWPacket & packetOut) const;

			/*! \brief Checks if Two Packets Are Equivalent
			 *
			 *  Checks if packetOut is equivalent to packetIn.
			 *
			 *  \param packetIn FSWPacket #1
			 *  \param packetOut FSWPacket #2
			 *  \return true if packetOut is equivalent to packetIn and false
			 *  otherwise.
			 */
			inline bool IsPacketSame(const FSWPacket & packetIn, const FSWPacket & packetOut) const;

			/*! \brief Checks the Message Queue for a Matching FSWPacket
			 *
			 *  Checks the internal message queue for a packet that matches
			 *  packetIn using the comparison function given by Check.  If
			 *  a matching packet is found, a pointer to the packet is
			 *  returned in packetOut.
			 *
			 *  \param packetIn FSWPacket to compare other packets with.
			 *  \param packetOut Returns a pointer to the matching packet.
			 *  \param Check Comparison function used to compare packetIn
			 *  and the other packets in the internal message queue.
			 *  \return true if a matching packet was found and false
			 *  otherwise.
			 *
			 *  \note packetOut is only guaranteed to point to a valid
			 *  packet if this function returns true.
			 */
			bool CheckQueueForMatchingPacket(const FSWPacket & packetIn,
											 FSWPacket * &packetOut,
											 DispatcherCheckType type);

            uint32_t DispatchToHardware(FSWPacket & packet);

            /*! \brief Struct for Holding Server Message Handler Information */
			struct DispatcherHandlerType
			{
				DispatcherHandlerType(MessageHandlerRegistry * regIn, Arbitrator * arbyIn)
					: registry(regIn), arby(arbyIn) { }

				MessageHandlerRegistry * registry;
				Arbitrator * arby;
			};

            /*! \brief MessageHandler Registry Map */
			std::map<LocationIDType, DispatcherHandlerType *> registryMap;

            /*! \brief Handle to the Dispatcher Message Queue */
			mqd_t queueHandleRX;
			struct mq_attr queueAttrRX;
			int qInitRX;
            static char * queueNameRX;

        private:

            struct DispatcherInterruptAlertType
            {
                uint8 pin;
                uint32 time;
            };

            /*! \brief Typedef for Pairs in the Registry Map */
            typedef std::pair<LocationIDType, DispatcherHandlerType *> PairType;

            /*! \brief Typedef for the Iterator Type of the Registry Map */
            typedef std::map<LocationIDType, DispatcherHandlerType *>::iterator IteratorType;

            /*! \brief Struct for Passing Parameters to InvokeHandler */
            struct DispatcherTaskParameter
            {
            	MessageHandlerRegistry * registry;
            	FSWPacket * packet;
            	FSWPacket * retPacket;
            	sem_t syncSem;
            	sem_t doneSem;
            };
			
			/*! \brief Initialize the Dispatcher Class
             *
             *  Initializes the operating system constructs needed for
             *  Dispatcher to work properly.
             *
             *  \return true if the initialization was successful and
             *  false otherwise.
             *
             *  \note This function MUST be called before GetInstance.
             *  Otherwise, GetInstance will fail and return NULL.
             */
            static void Initialize(void);
			
			/*! \brief Static Destructor for Dispatcher
             *
             *  Frees all internal memory use, frees all operating system
             *  objects used, and deletes the singleton instance pointer.
             */

            static void Destroy(void );

			
			/*! \brief Checks if Dispatcher Class is initialized
             *
             *  \return true if the initialization was successful and
             *  false otherwise.
             */
			bool IsFullyInitialized(void);

            /*! \brief Invokes a MessageHandler in a New Task
             *
             *  \param parameters Pointer to DispatcherTaskParameter struct.
             *
             *  \note parameters must ALWAYS point to a DispatcherTaskParameter
             *  struct.
             */
            static void * InvokeHandler(void * parameters);

			/*! \brief Destructor for Dispatcher */
			virtual ~Dispatcher(void );

            /*! \brief Constructor for Dispatcher */
            Dispatcher(void);

            /*! \brief Assignment Operator for Dispatcher */
            Dispatcher & operator=(const Dispatcher & source);
        };
    }
}

#endif  //_DISPATCHER_H
