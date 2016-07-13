/*
* GPSServer.cpp
*
*    Created: 7/6/12
* Written by: Conrad Hougen
*
*    Updated: 7/7/16
*     Author: Alex St. Clair
*
*/

#include "servers/GPSServer.h"
#include "servers/GPSHandlers.h"
#include "servers/GPSStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"

#include "core/ErrorMessage.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#include "HAL/GPS.h"
#include "HAL/RTC.h"
#include <math.h>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
		// FIXME: find the right directory
		const char * GPSServer::portname = (char *) "/dev/ttyS0";

		// handler for Health and status measurements
		static GPSHealthAndStatusHandler * gpsHSHandler;
		// GPS date/time handler
		static GPSTimeHandler * gpsTimeHandler;
		// handler for position requests
		static GPSPositionHandler * gpsPosHandler;
		// handler for position requests
		static GPSResetHandler * gpsRstHandler;
		// handler for errors
		static GPSErrorHandler * gpsErrorHandler;

		GPSServer::GPSServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			GPSDataHolder = new GPSData();
		}

		GPSServer::~GPSServer()
		{
			// Left Intentionally Blank
		}

		GPSServer & GPSServer::operator=(const GPSServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}

		void GPSServer::Initialize(void)
		{
			gpsHSHandler = new GPSHealthAndStatusHandler();
			gpsTimeHandler = new GPSTimeHandler;
			gpsPosHandler = new GPSPositionHandler();
			gpsRstHandler = new GPSResetHandler;
			gpsErrorHandler = new GPSErrorHandler;
		}

#ifdef TEST
		void GPSServer::Destroy(void)
		{
			delete gpsHSHandler;
			delete gpsTimeHandler;
			delete gpsPosHandler;
			delete gpsRstHandler;
			delete gpsErrorHandler;
		}
#endif

		bool GPSServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void GPSServer::Update(SystemModeEnum mode)
		{

		}

		bool GPSServer::RegisterHandlers()
		{
			bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_HS_CMD), gpsHSHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_TIME_CMD), gpsTimeHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_POSITION_CMD), gpsPosHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_RESET_CMD), gpsRstHandler);

			for(int opcode = GPS_CMD_MIN; opcode < GPS_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			for(int opcode = GPS_SUB_ERROR_MIN; opcode < GPS_SUB_ERROR_MAX; opcode++)
			{
				success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), gpsErrorHandler);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void GPSServer::SubsystemLoop(void)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			char buffer[350];
			int fd;

			fd = CreatePort();

			while(1)
			{
				uint64_t LastWakeTime = getTimeInMillis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// TODO: check where port is: ie. /dev/ttyS?
				//		 Give the port the right permissions?
				ReadData(buffer, fd);


				waitUntil(LastWakeTime, 100);
			}

		}

		// TODO: error handling
		int GPSServer::CreatePort(void){
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));


			int portfd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
			if(portfd == -1){
				logger->Log("GPSServer: Failed to open serial port!", LOGGER_LEVEL_FATAL);
			}

			if(tcgetattr(portfd, &port) < 0) {
				logger->Log("GPSServer: Problem with initial port attributes.", LOGGER_LEVEL_ERROR);
			}

			port.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
			port.c_cflag |= CLOCAL | CREAD | CS8;
			port.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
			port.c_lflag &= ~(ECHO | ECHONL | ISIG);
			port.c_lflag |= ICANON;

			if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
				logger->Log("GPSServer: Problem setting the baud rate!", LOGGER_LEVEL_FATAL);
			}

			if(tcsetattr(portfd, TCSANOW, &port) < 0){
				logger->Log("GPSServer: Problem setting port attributes!", LOGGER_LEVEL_ERROR);
			}

			return portfd;
		}

		double GPSServer::DistanceTo(double latitude1, double longitude1){
			double R = 6371000.0; //Radius of earth in meters
			double latitude2 = this->latitude;
			double longitude2 = this->longitude;

			//to radians
			latitude1 = latitude1*M_PI/180.0;
			longitude1 = longitude1*M_PI/180.0;
			latitude2 = latitude2*M_PI/180.0;
			longitude2 = longitude2*M_PI/180.0;

			double dlat = latitude2 - latitude1;
			double dlong = longitude2 - longitude1;

			double a = sin(dlat/2) * sin(dlat/2) + cos(latitude1)*cos(latitude2)*sin(dlong/2)*sin(dlong/2);
			double c = 2*atan2(sqrt(a), sqrt(1-a));
			return c*R;
		}

		void GPSServer::ReadData(char * buffer, int fd){
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

			char c, c1;
			uint8 readRet = 1;
			uint16 counter = 0;

			// Check that there's data
			if(read(fd,&c,1) != 0){
				logger->Log("GPSServer: Reading data from GPS", LOGGER_LEVEL_DEBUG);

				// Check the first character
				if(c != '#' || c != '$'){
					logger->Log("GPSServer: Data doesn't start with '#' or '$'", LOGGER_LEVEL_WARN);
				}else{
					buffer = {0};
					c1 = c;
					// read while there's more data, and ensure we don't overflow the buffer
					while(counter != 350 && readRet != 0){
						buffer[counter++] = c;
						readRet = read(fd,&c,1);
					}

					logger->Log("GPSServer: Read data from GPS, now processing...", LOGGER_LEVEL_DEBUG);

					for(int i = 0; i < counter; i++){
						if(i%30 == 0){
							printf("\n");
						}
						printf("%c", buffer[i]);
					}
					printf("\n");

					if(c1 == '#'){
						if(!BESTXYZProcess(buffer, counter)){
							logger->Log("GPSServer: Error processing BESTXYZ data!", LOGGER_LEVEL_ERROR);
						}
					}else if(c1 == '$'){
						if(!GPRMCProcess(buffer, counter)){
							logger->Log("GPSServer: Error processing GPRMC data!", LOGGER_LEVEL_ERROR);
						}
					}else{
						logger->Log("GPSServer: error with message!", LOGGER_LEVEL_WARN);
					}
				}
			}else{
				return;
			}
		}

		GPSData * GPSServer::GetGPSDataPtr(void)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				this->GiveLock();
				return GPSDataHolder;
			}
			return NULL;
		}
	}
}
