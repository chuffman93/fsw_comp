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

namespace AllStar{
namespace Servers{

struct BeaconData
{
	// Callsign? possibly "cosgc1as"
	float secondsSinceLastLogin;
	uint8 errorFlags;
	uint8 currenntMode;
	uint8 resetFlags;
	uint16 acsErrorFlags;

	// eps HS
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
};

class COMServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers(void);

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	COMServer(std::string nameIn, LocationIDType idIn);
	~COMServer(void);
	COMServer & operator=(const COMServer & source);

	// Member variables for handler registration
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	BeaconData * beaconData;

	// ------ State Machine ---------------------------------------------------
	void loopInit();
	void loopIdle();
	void loopCOMStart();
	void loopCOMHalf();
	void loopCOMFull();
	void loopCOMStop();
	void loopDiagnostic();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&COMServer::loopInit)
	STATE_MAP_ENTRY(&COMServer::loopIdle)
	STATE_MAP_ENTRY(&COMServer::loopCOMStart)
	STATE_MAP_ENTRY(&COMServer::loopCOMHalf)
	STATE_MAP_ENTRY(&COMServer::loopCOMFull)
	STATE_MAP_ENTRY(&COMServer::loopCOMStop)
	STATE_MAP_ENTRY(&COMServer::loopDiagnostic)
	STATE_MAP_ENTRY(&COMServer::loopReset)
	END_STATE_MAP

	enum COM_States {
		ST_INIT = 0,
		ST_IDLE,
		ST_COM_START,
		ST_COM_HALF,
		ST_COM_FULL,
		ST_COM_STOP,
		ST_DIAGNOSTIC,
		ST_RESET
		// BEACON, RX, TX ...
	};
};

}
}

#endif /* COMSERVER_H_ */
