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

	const static char * portname;

	double DistanceTo(double target[3]);

	bool GetECIData(uint8 buffer[GPSInertial::size]);

	void UpdateECEFData(GPSPositionTime * data);

	bool updateConfig(void);
	void bootConfig(void);

	uint8 numTracked;

private:
	bool IsFullyInitialized(void);

	GPSServer(std::string nameIn, LocationIDType idIn);
	~GPSServer();
	GPSServer & operator=(const GPSServer & source);

	int CreatePort(void);
	void RestartGPS(void);
	bool ConfigureGPS();

	static GPSPositionTime * GPSDataHolder;
	static GPSInertial * GPSInertialCoords;

	int GPSFileDescriptor;

	GPSReadType ReadData(char * buffer, int fd);

	void UpdateAndPropagate();
	void ECItoOE();
	void ECItoECEF();
	void ECEFtoECI();
	void WriteLockToFile();
	void bootLastLock();
	void UpdateOEfromConfig();
	// GPS Port configurations
	struct termios port;

	GPSConfig GPSConfiguration;
	static GPSLockType lastLock;
	int64 inertialTime;
	bool propagating; // flag for whether or not the GPS server is propagating
	bool noOE; // flag if we haven't gotten a lock or orbital elements from the ground
	bool timeKnown; // flag for if we know the time

	BEGIN_STATE_MAP
	END_STATE_MAP

};

}
}

#endif // GPSSERVER_H_
