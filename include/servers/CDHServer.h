/*
 * CDHServer.h
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#ifndef CDHSERVER_H_
#define CDHSERVER_H_

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/MessageHandler.h"
#include "core/Singleton.h"
#include "core/ModeManager.h"

#include "servers/SubsystemServer.h"

#include "util/StorageManager.h"

#include <sys/sysinfo.h>
#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"
#include "HAL/I2C/I2CDeviceManager.h"

using namespace AllStar::HAL;

namespace AllStar{
namespace Servers{

class CDHServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

	// Turn a subsystem on  (only ACS, COM, PLD, GPS)
	void subPowerOn(HardwareLocationIDType subsystem);

	// Turn a subsystem off (only ACS, COM, PLD, GPS)
	void subPowerOff(HardwareLocationIDType subsystem);

	//control the reset lines (ACS, COM, PLD, EPS only)
	void resetAssert(HardwareLocationIDType subsystem);
	void resetDeassert(HardwareLocationIDType subsystem);

	// Allows for easy look into memory usage
	struct sysinfo si;
	I2CDeviceManager * devMngr;
	StorageManager * storMngr;
	bool subsystemPowerStates[HARDWARE_LOCATION_MAX];

private:
	static void Initialize(void);
#ifdef TEST
	static void Destroy(void);
#endif
	bool IsFullyInitialized(void);

	CDHServer(std::string nameIn, LocationIDType idIn);
	~CDHServer();
	CDHServer & operator=(const CDHServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	// Modes
	void loopInit();
	void loopMonitor();

	// Gather CDH data at a given frequency
	void readHealth(uint8 frequency, uint32 timeUnit);

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&CDHServer::loopInit)
	STATE_MAP_ENTRY(&CDHServer::loopMonitor)
	END_STATE_MAP

	enum CDH_States {
		ST_INIT = 0,
		ST_MONITOR
	};
};

}
}

#endif //CDHSERVER_H_
