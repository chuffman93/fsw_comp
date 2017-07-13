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
#include "servers/GPSStdTasks.h"
#include "servers/SubsystemServer.h"

#include <termios.h>
#include <unistd.h>

namespace AllStar {
namespace Servers {

typedef enum GPSRead {
	GPS_NO_DATA,
	GPS_LOCK,
	GPS_NO_LOCK
} GPSReadType;

struct GPS_BESTXYZ {
	int GPSWeek;
	float GPSSec;
	double posX, posY, posZ, velX, velY, velZ;
	uint8 numTracked;
};

struct GPS_GPRMC {
	double latitude;
	double longitude;
};

class GPSServer : public SubsystemServer, public AllStar::Core::Singleton {
	friend class AllStar::Core::Factory;

public:
	void SubsystemLoop(void);

	bool RegisterHandlers(void);

	GPS_BESTXYZ * GetGPSDataPtr(void);
	GPS_GPRMC * GetGPSCoordsPtr(void);

	const static char * portname;

	double DistanceTo(double latitude1, double longitude1);

	GPS_BESTXYZ * GPSDataHolder;
	GPS_GPRMC * GPSCoordsHolder;

private:
	bool IsFullyInitialized(void);

	GPSServer(std::string nameIn, LocationIDType idIn);
	~GPSServer();
	GPSServer & operator=(const GPSServer & source);

	int CreatePort(void);

	GPSReadType ReadData(char * buffer, int fd);

	// GPS Port configurations
	struct termios port;

	BEGIN_STATE_MAP
	END_STATE_MAP

};

}
}

#endif // GPSSERVER_H_
