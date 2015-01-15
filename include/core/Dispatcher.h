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

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Length of the Dispatcher Queue */
        #define DISPATCHER_QUEUE_LENGTH 		10 //FIXME:should be 32

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
        #define DISPATCHER_LISTEN_MAX_DELAY		(1000000) //was 1000
        #endif // DISPATCHER_LISTEN_MAX_DELAY
		
		/*! \brief Delay Between Protocol SPI Bytes */
		#define PROTOCOL_SPI_DELAY      (200)
		
        /*! \brief Dispatcher Maximum Response Tries
         *
         *  Number of times the Dispatcher checks the queue for a packet
         *  before failing when it is waiting for a response from a
         *  dispatched packet.
         */
        #define DISPATCHER_MAX_RESPONSE_TRIES	100
		
		/*! \brief Dispatcher delay loop */
		#define	MAX_DISPATCHER_LOOP		500

    	/*! \brief Max packet size in bytes */
		#define MAX_PACKET_SIZE			65551

    	/* \brief Signals used for SPI communication*/
		#define SENT_COMPLETE_SIG		SIGUSR1
		#define RECEIVED_PACKET_SIG		SIGUSR2

        /*! \brief Dispatcher Status Enumeration
         *
         *  Enumerates the possible return values from the
         *  WaitForDispatchResponse method.
         */
        enum DispatcherStatusEnum
        {
                /*! \brief Response Receive Successful */
                DISPATCHER_STATUS_OK = 0,
                /*! \brief Dispatched FSWPacket Not Received by Destination*/
                DISPATCHER_STATUS_MSG_NOT_RCVD,
                /*! \brief Dispatched FSWPacket Received But No Response Received */
                DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT,
                /*! \brief Maximum Return Value */
                DISPATCHER_STATUS_MAX
        };

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
//			static char * getQueueName(void);
//			static mqd_t * getQueueHandle(void);
//			static struct mq_attr * getQueueAttr(void);
			/*! \brief Give the semaphore to the Dispatcher Task from
             *  an ISR.
             *
             *  \param woken Pointer to a portBASE_TYPE that indicates
             *  whether a task was woken due to the call.  If it was,
             *  the ISR should cause a context switch.
             *
             *  \note This method must ONLY be called from an ISR.
             */
			//TODO:FIX THIS
           // static void EnqueueSubsystemIntFromISR(uint8 pin, uint32 time, portBASE_TYPE * woken);



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
            bool Dispatch(const FSWPacket & packet);

            /*! \brief Waits for a Response after a FSWPacket Has Been Dispatched
             *
             *  Waits for the response to packet, which should have been
             *  previously sent to another server by calling Dispatch.
             *  If the response is in the queue, then it is removed from the
             *  queue and returned in returnMessage.  Otherwise, if the initial
             *  packet is still in the queue, it is removed.
             *
             *  \param packet FSWPacket sent via Dispatch.
             *  \param returnMessage Output variable that is set to the
             *  response, if the operation was successful.
             *  \return \ref DISPATCHER_STATUS_OK if a response was
             *  successfully obtained,<br>
			 *  \ref DISPATCHER_STATUS_MSG_NOT_RCVD if the packet that was
			 *  previously Dispatched was not received by the other server,<br>
			 *  \ref DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT if the packet
			 *  that was previously Dispatched was received by the other
			 *  server but not response was received.
             *
             *  \note returnMessage is only guaranteed to contain useful
             *  information if DISPATCHER_STATUS_OK is returned.
             *
             *  \note If this method finds that the previously Dispatched
             *  packet was not received, it removes that packet from
             *  the internal message queue.
             */
            DispatcherStatusEnum WaitForDispatchResponse(const FSWPacket & packet, ReturnMessage & returnMessage);

            void printString(const uint8 * str);

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
            bool Listen(LocationIDType serverID);


        private:
            /*! \brief Struct for Holding Server Message Handler Information */
            struct DispatcherHandlerType
            {
            	DispatcherHandlerType(MessageHandlerRegistry * regIn, Arbitrator * arbyIn)
					: registry(regIn), arby(arbyIn) { }

            	MessageHandlerRegistry * registry;
            	Arbitrator * arby;
            };

            struct DispatcherInterruptAlertType
            {
                uint8 pin;
                uint32 time;
            };

            /*! \brief Typedef for Pairs in the Registry Map */
            typedef std::pair<LocationIDType, DispatcherHandlerType *> PairType;

            /*! \brief Typedef for the Iterator Type of the Registry Map */
            typedef std::map<LocationIDType, DispatcherHandlerType *>::iterator IteratorType;

            /*! \brief Typedef for a FSWPacket Comparison Function */
            typedef bool (Dispatcher::* PacketCheckFunctionType)(const FSWPacket & packetIn, const FSWPacket & packetOut) const;

            /*! \brief Struct for Passing Parameters to InvokeHandler */
            struct DispatcherTaskParameter
            {
            	/*! \brief Registry from which to Invoke Handler */
            	MessageHandlerRegistry * registry;

            	/*! \brief FSWPacket to Pass to the MessageHandler */
            	FSWPacket * packet;

            	/*! \brief Return Value of the MessageHandler */
            	ReturnMessage * retMsg;

            	sem_t syncSem;

            	sem_t doneSem;
            };

            /*! \brief Handle to the Dispatcher Subsystem Task Queue */
            mqd_t subsystemQueueHandle;
            struct mq_attr subsystemQueueAttr;
            static  char * subsystemQueueName;
            int subQinit;

            /*! \brief Handle to the Dispatcher Message Queue */
            mqd_t queueHandle;
            struct mq_attr queueAttr;
            static char * queueName;
            int qInit;

            /*! \brief MessageHandler Registry Map */
            std::map<LocationIDType, DispatcherHandlerType *> registryMap;
			
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
            		                         PacketCheckFunctionType Check);

            /*! \brief Sends an Error Response to a FSWPacket
             *
             *  Sends a response packet to the source field in the given
             *  packet.  The message in the packet is a ReturnMessage
             *  that wraps an ErrorMessage with the given opcode.
             *
             *  \param errorCode ErrorMessage opcode.
             *  \param packet FSWPacket that is being responded to.
             *
             *  \return true if the response was sent successfully and false
             *  otherwise.
             *
             *  \note If the response fails, then the original packet is put
             *  back on the message queue so that it can be handled again.
             */
            bool SendErrorResponse(ErrorOpcodeEnum errorCode, FSWPacket * packet,VariableTypeData data);

            /*! \brief Invokes a MessageHandler in a New Task
             *
             *  \param parameters Pointer to DispatcherTaskParameter struct.
             *
             *  \note parameters must ALWAYS point to a DispatcherTaskParameter
             *  struct.
             */
            static void * InvokeHandler(void * parameters);

#ifndef WIN32
            /*! \brief Handles SIGUSR1 when send is complete
             *
             *	Changes value of sent to true to indicate that a packet has been
             *	sent so that DispatchToHardware can return
             *
             */
            void sendComplete(int signum);

            /*! \brief Signal handler that Places packet from hardware on queue
             *
             *	Copies message that was received over SPI onto the dispatcherQueue
             *
             */
            void receivedPacket(int signum);

            /*! \brief Dispatch a Message to Hardware
             *
             *  Sends a message to a hardware location and then receives the
             *  response and inserts it into the Dispatcher queue so that it
             *  can be handled by the appropriate server.
             *
             */

            uint32_t DispatchToHardware(HardwareLocationIDEnum loc, const FSWPacket & packet);

            /*! \brief Returns the Device and Chip for a Hardware Location
             *
             *  \param loc Location of Hardware Device.
             *  \param dev SPI Device for Given Location (Return Value).
             *  \param chip Chip Number for Given Location (Return Value).
             *
             *  \return true if successful and false otherwise.
             */
           // bool GetHardwareDeviceFromLocation(HardwareLocationIDEnum loc, SPIDeviceEnum & dev, uint8_t & chip);

            /*! \brief Sends a FSWPacket to the Specified Hardware.
             *
             *  \param loc Hardware Location to Write to.
             *  \param packet FSWPacket to Send.
             *
             *  \return 0 if successful and the line number of failure otherwise.
             */
           // uint32_t SendPacketToHardware(SPIDeviceEnum dev, const FSWPacket & packet);

            /*! \brief Receives a FSWPacket from Hardware and Dispatches it.
             *
             *  \param loc Hardware Location to Read From.
             *
             *  \return 0 if successful and the line number of failure otherwise.
             */
           // uint32_t GetPacketFromHardware(SPIDeviceEnum dev, FSWPacket & packetOut);



#endif // WIN32

			/*! \brief Destructor for Dispatcher */
			virtual ~Dispatcher(void );

            /*! \brief Constructor for Dispatcher */
            Dispatcher(void);

            /*! \brief Copy Constructor for Dispatcher */
            Dispatcher(const Dispatcher & source);

            /*! \brief Assignment Operator for Dispatcher */
            Dispatcher & operator=(const Dispatcher & source);
        };
    }
}

#endif  //_DISPATCHER_H
