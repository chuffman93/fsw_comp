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
#include "servers/GPSStdTasks.h"
#include "util/FileHandler.h"
#include "servers/SubsystemServer.h"

#include <termios.h>
#include <unistd.h>

namespace AllStar{
namespace Servers{

struct GPS_BESTXYZ{
	char header[100], message[250], crc[10], MessageID[10], Port[5], TimeStatus[15], pSolStatus[15], pSolType[15], vSolStatus[15],
	vSolType[15], stnID[10];
	int SequenceNum;
	int GPSWeek, numTracked, numSolution;
	float GPSSec, IdleTime, GPSTime;
	double posX, posY, posZ, velX, velY, velZ;
	float stdDevPX, stdDevPY, stdDevPZ, stdDevVX, stdDevVY, stdDevVZ;
	float posECIX, posECIY, posECIZ, velECIX, velECIY, velECIZ;
	float latency,diffAge,solAge;
	uint32 round_seconds;
};

struct GPS_GPRMC{
	char utc[9];
	double latitude;
	double longitude;
};

class GPSServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	void SubsystemLoop(void);

	bool RegisterHandlers(void);

	GPS_BESTXYZ * GetGPSDataPtr(void);
	GPS_GPRMC * GetGPSCoordsPtr(void);

	const static char * portname;

	double DistanceTo(double latitude1, double longitude1);

	int GetWeek(void);
	float GetSeconds(void);
	uint32 GetRoundSeconds(void);

	bool gpsResponsive;
	GPS_BESTXYZ * GPSDataHolder;
	GPS_GPRMC * GPSCoordsHolder;

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	GPSServer(std::string nameIn, LocationIDType idIn);
	~GPSServer();
	GPSServer & operator=(const GPSServer & source);

	int CreatePort(void);

	bool ReadData(char * buffer, int fd);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	// GPS Port configurations
	struct termios port;

	BEGIN_STATE_MAP
	END_STATE_MAP

};

}
}

#endif // GPSSERVER_H_
