/*
 * COMServer.h
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 *      Modified: Brian Campuzano
 */

#ifndef COMSERVER_H_
#define COMSERVER_H_

#include "servers/SubsystemServer.h"
#include "servers/COMHandlers.h"

#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/Factory.h"


#define COM_DEBUG			1

#if COM_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#define DEBUG_CHAR(c) cout << (int) c << " ";
#else
#define DEBUG_COUT(m)
#define DEBUG_CHAR(c)
#endif // DEBUG

namespace Phoenix
{
	namespace Servers
	{

		enum COMStateEnum
		{
			BEACON_STATE = 1,
			COMMAND_STATE,
			SEND_STATE,
			RECEIVE_STATE,
			NUM_STATES
		};

		// Maximum number of files that can be down-linked at a time
		#define MAX_DOWNLINK_FILES 		20
		// Maximum number of files that can be up-linked at a time
		#define MAX_UPLINK_FILE			10

		#define KERMIT_MAX_DELAY		((portTickType) 100)
		
		#define COM_INPUT_BUFFER_LENGTH 10240
		
		/*! \brief Struct for Passing Parameters to KermitTask */
		struct KermitReceiveParameter
		{
			LocationIDType server;
			
			sem_t syncSem;

			sem_t doneSem;
			
			//CircularBuffer * inBuf;
			
			char * fileIn;
			
			char * fileOut;
		};
		
		/*! \brief Struct for Passing Parameters to KermitTask */
		struct KermitSendParameter
		{
			LocationIDType server;
			
			/*! \brief file list to Pass to the SendKermitTask */
			//UCHAR ** fileList;
			
			int numFiles;

			sem_t syncSem;

			sem_t doneSem;
			
			//CircularBuffer * inBuf;
			
			char * fileIn;
			
			char * fileOut;
		};
		
		struct BeaconData
		{
			//Callsign
			//const static char callsign[] = "cosgc1as";
			float secondsSinceLastLogin;
			//bitmap for error flags
			uint8 errorFlags;
			//bitmap for current mode
			uint8 currenntMode;
			//bitmap for reset flags
			uint8 resetFlags;
			
			//bitmap for ACS error flags
			uint16 acsErrorFlags;
			
			//eps HS
			int16 epsBattVoltage;
			int16 epsBattCurrent;
			uint16 epsStateOfCharge;
			int16 epsBattRemainingCapacity;
			int16 epsCycleCount;
			int16 epsBatteryManagerStatus;
			
			//com HS
			uint16 comSignalStrength;
			uint8 comTransmitPower;
			uint8 comTXPLLFlags;
			int8 comPATemp;
			uint8 comRXIFDetect;
			uint8 comRXGain;
			uint8 com5VCurrentSense;
			uint8 com5VVoltageSense;
			
			//gps HS
			uint8 gpsFlags;
			float gpsPosX;
			float gpsPosY;
			float gpsPosZ;
			float gpsVelX;
			float gpsVelY;
			float gpsVelZ;
			float gpsSolutionAge;
			
			//const static char eol[] = "eol";
		};

		class COMServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call COMServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
		public:
			//void taskRunCOM(void * params);
		
			void SubsystemLoop(void);
			void Update(const Phoenix::Core::SystemMode * mode);
			bool RegisterHandlers(void);

			bool SetState(COMStateEnum state);
			COMStateEnum GetState(void);
			
			bool SetParseFlag(void);
			

			//TODO:replace kermit
			//bool SetSendTaskHandle(xTaskHandle * sendHandle);
			//bool SetReceiveTaskHandle(xTaskHandle * recHandle);
			bool SetSendParameters(KermitSendParameter * sendParameters);
			bool SetReceiveParameters(KermitReceiveParameter * recParameters);
			
			//CircularBuffer * GetInputBuffer(void);
			char * GetFileInputBuffer(void);
			char * GetFileOuputBuffer(void);
			
			bool SetKermitRunFlag(bool state);
			bool GetKermitRunFlag(void);
			BeaconData * GetBeaconHolder(void);
			void UpdateEPSHS(int16 battVoltage, int16 battCurrent, uint16 stateOfCharge, int16 battRemainingCapacity,
				int16 cycleCount, int16 batteryManagerStatus);
			void UpdateCOMHS(uint16 lwipPacketsTx,uint16 lwipPacketsRx,uint16 lwipPacketsDrop,uint16 lwipPacketsErrors,
				uint16 lwipEtharpTx,uint16 lwipEtharpRx,uint16 lwipEtharpDrop,uint16 lwipEtharpErrors,
				uint16 lwipIPTx,uint16 lwipIPRx,uint16 lwipIPDrop,uint16 lwipIPErrors,
				uint16 lwipUDPTx,uint16 lwipUDPRx,uint16 lwipUDPDrop,uint16 lwipUDPErrors,
				uint16 lwipTCPTx,uint16 lwipTCPRx,uint16 lwipTCPDrop,uint16 lwipTCPErrors,
				uint16 lwipMemAvail,uint16 lwipMemUsed,uint16 lwipMemMax,uint16 lwipMemErrors,uint16 lwipMemIllegal,
				uint16 comState,uint16 statBits, uint16 signalStrength,
				uint8 transmitPower, uint8 RXIFDetect, uint8 RXGain, uint8 currentSense, uint8 voltageSense);
			void UpdateGPSHS(uint8 flags, float posX, float posY, float posZ, float velX, float velY,
				float velZ, float solutionAge);

		private:
			/*! \brief Initialize the COMServer Class
			*
			*  Initializes the operating system constructs needed for
			*  COMServer to work properly.
			*/
			static void Initialize(void);
		
			/*! \brief Static Destructor for COMServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef HOST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if COMServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			COMServer(std::string nameIn, LocationIDType idIn);
			~COMServer(void);
			COMServer & operator=(const COMServer & source);

			/*! \brief Member variables needed to register message handlers */
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			/*! \briefState of the COM State Machine */
			COMStateEnum COMCurrentState;

			/*! \brief Flag used to set when to parse the input buffer for ground station commands */
			bool parseFlag;
			
			/*! \brief Run flag used to tell kermit to terminate */
			bool kermitRunFlag;

			/*! \brief Handles for the kermit tasks */
			//TODO:replace kermit
			//xTaskHandle * sendTaskHandle;
			//xTaskHandle * recTaskHandle;
			
			/*! \brief task parameters for kermit tasks */
			KermitSendParameter * sendParams;
			KermitReceiveParameter * recParams;

			/*! \brief COM input buffer. */
			//CircularBuffer * inputBuffer;
			
			BeaconData * beaconData;
			
			/*! \brief COM file buffers. */
			char * fileInputBuffer;
			char * fileOutputBuffer;
			
			/*! \brief COM mode functions. */
			void COMAccessMode(Phoenix::Core::ModeManager * modeManager);		// 0
			void COMStartupMode(Phoenix::Core::ModeManager * modeManager);		// 1
			void COMBusMode(Phoenix::Core::ModeManager * modeManager);			// 2
			void COMPayloadMode(Phoenix::Core::ModeManager * modeManager);		// 3
			void COMErrorMode(Phoenix::Core::ModeManager * modeManager);		// 4
			void COMComMode(Phoenix::Core::ModeManager * modeManager);			// 5
			
			/*! \brief COM mode functional array. */
			void(COMServer::*modeArray[6])(Phoenix::Core::ModeManager * modeManager);
		};
	}
}

#endif /* COMSERVER_H_ */
