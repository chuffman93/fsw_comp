/*
 * EPSServer.h
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 *  Updated: Alex St. Clair
 */

#ifndef EPSSERVER_H_
#define EPSSERVER_H_

#include "servers/SubsystemServer.h"
#include "core/StdTypes.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "util/FileHandler.h"

namespace AllStar{
namespace Servers{

struct EPSStatus{
	uint16 current3v3;
	uint16 voltage3v3;
	uint16 currentVbat;
	uint16 voltageVbat;
	uint16 current12v;
	uint16 voltage12v;
	uint16 remainingCapacity;
	uint16 battCurrent;
	uint16 battVoltage;
	uint16 battStatus;
	uint16 frangCurrent;
	uint16 frangVoltage;
	uint16 convCurrentX;
	uint16 convThreshX;
	uint16 convCurrentY;
	uint16 convThreshY;
	uint16 convCurrentW;
	uint16 convThreshW;
};

class EPSServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	bool CheckHealthStatus();

	struct EPSConfig{
		uint16 minCOMCharge;
		uint16 minPLDCharge;
		uint16 minDSTCharge;
		uint8 numItems;
	};

	EPSStatus EPSState;
	EPSConfig EPSConfiguration;

private:

	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	EPSServer(std::string nameIn, LocationIDType idIn);
	~EPSServer(){}
	EPSServer & operator=(const EPSServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	//Modes
	void loopInit();
	void loopMonitor();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&EPSServer::loopInit)
	STATE_MAP_ENTRY(&EPSServer::loopMonitor)
	STATE_MAP_ENTRY(&EPSServer::loopReset)
	END_STATE_MAP

	enum EPS_States{
		ST_INIT = 0,
		ST_MONITOR,
		ST_RESET
	};

};

}
}

#endif /* EPSSERVER_H_ */
