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
#include "core/Singleton.h"
#include "core/ModeManager.h"

#include "servers/SubsystemServer.h"
#include "servers/Structs.h"
#include "HAL/I2C/PowerMonitor.h"

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

	void bootConfig(); // read in configs on bootup
	bool updateConfig(); // update configs after COM pass

	static CDHConfig CDHConfiguration;
	CDHStatus CDHState;

	PowerMonitor_Data CDH_powermonitors[4];

	union hs_union {
	  float hs_buffer[32];
	  struct {
	    float com3V3volt;
	    float com3V3curr;
	    float comBATvolt;
	    float comBATcurr;
	    float com12Vvolt;
	    float com12Vcurr;
	    float acs3V3volt;
	    float acs3V3curr;
	    float acsBATvolt;
	    float acsBATcurr;
	    float acs12Vvolt;
	    float acs12Vcurr;
	    float prop3V3volt;
	    float prop3V3curr;
	    float propBATvolt;
	    float propBATcurr;
	    float prop12Vvolt;
	    float prop12Vcurr;
	    float pld3V3volt;
	    float pld3V3curr;
	    float pldBATvolt;
	    float pldBATcurr;
	    float pld12Vvolt;
	    float pld12Vcurr;
	    float gps3V3volt;
	    float gps3V3curr;
	    float gpsBATvolt;
	    float gpsBATcurr;
	    float aux3V3volt;
	    float aux3V3curr;
	    float auxBATvolt;
	    float auxBATcurr;
	  };
	};

	hs_union CDH_hotswaps;

	float temperatures[4][16];

	int32 lastHSTLog;

	// Allows for easy look into memory usage
	I2CDeviceManager * devMngr;
	bool subsystemPowerStates[HARDWARE_LOCATION_MAX];

private:
	bool IsFullyInitialized(void);

	CDHServer(std::string nameIn, LocationIDType idIn);
	~CDHServer();
	CDHServer & operator=(const CDHServer & source);

	// Gather CDH data at a given frequency
	void readHealth();
	int healthCount;

	// Modes
	void loopInit();
	void loopMonitor();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&CDHServer::loopInit)
	STATE_MAP_ENTRY(&CDHServer::loopMonitor)
	STATE_MAP_ENTRY(&CDHServer::loopReset)
	END_STATE_MAP

	enum CDH_States {
		ST_INIT = 0,
		ST_MONITOR,
		ST_RESET
	};
};

}
}

#endif //CDHSERVER_H_
