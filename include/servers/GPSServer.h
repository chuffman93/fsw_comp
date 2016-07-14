/*
* GPSServer.h
*
*  Created on: July 3, 2012
*      Author: Conrad Hougen
*
*     Updated: 7/7/16
*      Author: Alex St. Clair
*/

#ifndef GPSSERVER_H_
#define GPSSERVER_H_

#include "core/StdTypes.h"
#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "servers/GPSHandlers.h"
#include "servers/GPSStdTasks.h"
#include "util/FileHandler.h"

#include <termios.h>
#include <unistd.h>

namespace Phoenix
{
	namespace Servers
	{
		struct BESTXYZ{
			char header[100], message[250], crc[10], MessageID[10], Port[5], TimeStatus[15], pSolStatus[15], pSolType[15], vSolStatus[15],
					vSolType[15], stnID[10];
			int SequenceNum;
			int GPSWeek, numTracked, numSolution;
			float GPSSec, IdleTime, GPSTime;
			double posX, posY, posZ, velX, velY, velZ;
			float stdDevPX, stdDevPY, stdDevPZ, stdDevVX, stdDevVY, stdDevVZ;
			float posECIX, posECIY, posECIZ, velECIX, velECIY, velECIZ;
			float latency,diffAge,solAge;
		};

		struct GPRMC{
			char utc[9];
			double latitude;
			double longitude;
		};

		class GPSServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call GPSServer's private constructor
			*/
			friend class Phoenix::Core::Factory;

		public:

			void SubsystemLoop(void);

			void Update(const Core::SystemMode * mode);

			/*! \brief Registers this server's message handlers.
			*
			*  \note Dispatcher must be initialized before this
			*      function can be called.
			*
			*  Registers any message handlers with the member
			*  MessageHandlerRegistry and modifies permissions
			*  with the member Arbitrator. Then those two members
			*  are added to the Dispatcher.
			*/
			bool RegisterHandlers(void);

			BESTXYZ * GetGPSDataPtr(void);
			GPRMC * GetGPSCoordsPtr(void);

			const static char * portname;

			double DistanceTo(double latitude1, double longitude1);

			double latitude;
			double longitude;

		private:
			/*! \brief Initialize the GPSServer Class
			*
			*  Initializes the operating system constructs needed for
			*  GPSServer to work properly.
			*/
			static void Initialize(void);

			/*! \brief Static Destructor for GPSServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef TEST
			static void Destroy(void);
#endif

			/*! \brief Checks if GPSServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);

			GPSServer(std::string nameIn, LocationIDType idIn);
			~GPSServer();
			GPSServer & operator=(const GPSServer & source);

			int CreatePort(void);

			void ReadData(char * buffer, int fd);
			void GPSMessageProcess(Phoenix::Core::ReturnMessage * retMsg);

			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			BESTXYZ * GPSDataHolder;
			GPRMC * GPSCoordsHolder;

			/* GPS Port configurations */
			struct termios port;

		};
	}
}

#endif /* GPSSERVER_H_ */
