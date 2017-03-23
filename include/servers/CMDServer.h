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
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include <queue>

namespace AllStar{
namespace Servers{

#define DOWNLINK_DIRECTORY		"/home/root/downlink"
#define UPLINK_DIRECTORY		"/home/root/uplink"
#define IMMED_DIRECTORY			"/home/root/immed"
#define SOT_PATH				UPLINK_DIRECTORY  "/SOT.txt"
#define DRF_PATH				UPLINK_DIRECTORY  "/DRF.txt"
#define DLT_PATH				UPLINK_DIRECTORY  "/DLT.txt"
#define PPE_PATH				UPLINK_DIRECTORY  "/PPE.txt"
#define SCHEDULE_PATH			UPLINK_DIRECTORY  "/SCH.txt"
#define EOT_PATH				UPLINK_DIRECTORY  "/EOT.txt"
#define IEF_PATH				UPLINK_DIRECTORY  "/IEF.txt"
#define UPLK_PASSWORD			"P0!@rCube\n"
#define UPLK_PASSWORD_SKIP		"P0!@rCube_1\n"
#define UPLK_PASSWORD_SEND		"P0!@rCube_2\n"

class CMDServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	static int subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

	struct CMDConfig{
		int32 resetPeriod;
		int32 fileChunkSize;
		int32 maxDownlinkSize;
		int32 beaconPeriod;
		int32 increasedBeaconPeriod;
	};

	union beaconUnion{
		struct beaconStruct {
			SystemModeEnum currentMode;
			uint8 subPowerState;
			int32 uptime;
			EPSStatus epsHS;
			ACSStatus acsHS;
			CDHStatus cdhHS;
			PLDStatus pldHS;
		} bStruct;
		uint8 bArray[sizeof(beaconStruct)];
	} beacon;

	CMDConfig CMDConfiguration;

	bool CheckForBeacon(void);

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

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
