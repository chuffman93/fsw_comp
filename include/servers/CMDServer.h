/*
 * CMDServer.h
 *
 *  Created on: Feb 12, 2013
 *      Author: Caitlyn
 *  Modified:
 *
 *     Updated: November 2016
 *  	Author: Alex St. Clair
 */

#ifndef CMDSERVER_H_
#define CMDSERVER_H_

#include "servers/SubsystemServer.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/CDHServer.h"
#include "servers/PLDServer.h"
#include "servers/FileSystem.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include <queue>

namespace AllStar{
namespace Servers{

class CMDServer : public SubsystemServer, public AllStar::Core::Singleton {
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

	void bootConfig(); // read in configs on bootup

	bool updateConfig(); // update configs after COM pass

	struct BeaconStruct {
		SystemModeEnum currentMode;
		uint8 subPowerState;
		int32 uptime;
		EPSStatus epsHS;
		ACSStatus acsHS;
		CDHStatus cdhHS;
		PLDStatus pldHS;
	};

	void serializeBeacon(uint8 * buf, std::size_t size);

	static CMDConfig CMDConfiguration;
	BeaconStruct beacon;

	bool CheckForBeacon(void);

private:
	bool IsFullyInitialized(void);

	CMDServer(std::string nameIn, LocationIDType idIn);
	~CMDServer();
	CMDServer & operator=(const CMDServer & source);

	// Additional member variables
	int numFilesDWN;
	int currFileNum;
	int32 startTime;
	bool beaconValid;

	// Modes
	void loopInit();
	void loopIdle();
	void loopPassPrep();
	void loopLogin();
	void loopVerboseHS();
	void loopUplink();
	void loopImmediateExecution();
	void loopDownlinkPrep();
	void loopDownlink();
	void loopPostPass();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&CMDServer::loopInit)
	STATE_MAP_ENTRY(&CMDServer::loopIdle)
	STATE_MAP_ENTRY(&CMDServer::loopPassPrep)
	STATE_MAP_ENTRY(&CMDServer::loopLogin)
	STATE_MAP_ENTRY(&CMDServer::loopVerboseHS)
	STATE_MAP_ENTRY(&CMDServer::loopUplink)
	STATE_MAP_ENTRY(&CMDServer::loopImmediateExecution)
	STATE_MAP_ENTRY(&CMDServer::loopDownlinkPrep)
	STATE_MAP_ENTRY(&CMDServer::loopDownlink)
	STATE_MAP_ENTRY(&CMDServer::loopPostPass)
	STATE_MAP_ENTRY(&CMDServer::loopReset)
	END_STATE_MAP

	enum CMD_States {
		ST_INIT = 0,
		ST_IDLE,
		ST_PASS_PREP,
		ST_LOGIN,
		ST_VERBOSE_HS,
		ST_UPLINK,
		ST_IMMEDIATE_EXECUTION,
		ST_DOWNLINK_PREP,
		ST_DOWNLINK,
		ST_POST_PASS,
		ST_RESET,
	};

	bool CheckHealthStatus(void);
	void CreateBeacon(void);
};

}
}

#endif /* CMDSERVER_H_ */
