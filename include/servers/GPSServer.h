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
#include "servers/Structs.h"

extern "C" {
	#include "util/propagator/AllStarOrbitProp.h"
	#include "util/propagator/gpsFrameRotation.h"
	#include "util/propagator/OrbitalMotionAllStar.h"
}

#include <termios.h>
#include <unistd.h>

namespace AllStar {
namespace Servers {

typedef enum GPSRead {
	GPS_NO_DATA,
	GPS_LOCK,
	GPS_NO_LOCK
} GPSReadType;

struct GPSLockType {
	classicElements elements;
	float sysTime;
};

class GPSServer : public SubsystemServer, public AllStar::Core::Singleton {
	friend class AllStar::Core::Factory;

public:
	void SubsystemLoop(void);

	bool RegisterHandlers(void);

	GPSPositionTime * GetGPSDataPtr(void);
	GPSCoordinates * GetGPSCoordsPtr(void);

	const static char * portname;

	double DistanceTo(double latitude1, double longitude1);

	uint8 numTracked;
	static GPSPositionTime * GPSDataHolder;
	static GPSCoordinates * GPSCoordsHolder;
	static GPSInertial GPSInertialCoords;

private:
	bool IsFullyInitialized(void);

	GPSServer(std::string nameIn, LocationIDType idIn);
	~GPSServer();
	GPSServer & operator=(const GPSServer & source);

	int CreatePort(void);

	GPSReadType ReadData(char * buffer, int fd);

	void UpdateAndPropagate();
	void ECItoOE();
	void ECEFtoECI();

	// GPS Port configurations
	struct termios port;

	static GPSLockType lastLock;
	bool propagating; // flag for whether or not the GPS server is propagating
	bool noOE; // flag if we haven't gotten a lock or orbital elements from the ground

	BEGIN_STATE_MAP
	END_STATE_MAP

};

}
}

#endif // GPSSERVER_H_
