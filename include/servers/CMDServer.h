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
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "util/FileHandler.h"
#include <queue>

namespace AllStar{
namespace Servers{

#define DOWNLINK_DIRECTORY	"/home/root/downlink"
#define UPLINK_DIRECTORY	"/home/root/uplink"
#define SOT_PATH			UPLINK_DIRECTORY  "/SOT.txt"
#define DRF_PATH			UPLINK_DIRECTORY  "/DRF.txt"
#define DLT_PATH			UPLINK_DIRECTORY  "/DLT.txt"
#define PPE_PATH			UPLINK_DIRECTORY  "/PPE.txt"
#define SCHEDULE_PATH		UPLINK_DIRECTORY  "/SCH.txt"
#define EOT_PATH			UPLINK_DIRECTORY  "/EOT.txt"

#define FILE_CHUNK_SIZE		"10k"	// size of chunks to make (ie. 500, 5k, 10M, etc.)

class CMDServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	static int subsystem_acp_protocol[HARDWARE_LOCATION_MAX];
private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	CMDServer(std::string nameIn, LocationIDType idIn);
	~CMDServer();
	CMDServer & operator=(const CMDServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	// Additional member variables
	int numFilesDWN;
	int currFileNum;
	int32 resetPeriod;
	int32 startTime;

	// Modes
	void loopInit();
	void loopIdle();
	void loopDiagnostic();
	void loopPassPrep();
	void loopLogin();
	void loopVerboseHS();
	void loopUplink();
	void loopDownlinkPrep();
	void loopDownlink();
	void loopPostPass();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&CMDServer::loopInit)
	STATE_MAP_ENTRY(&CMDServer::loopIdle)
	STATE_MAP_ENTRY(&CMDServer::loopDiagnostic)
	STATE_MAP_ENTRY(&CMDServer::loopPassPrep)
	STATE_MAP_ENTRY(&CMDServer::loopLogin)
	STATE_MAP_ENTRY(&CMDServer::loopVerboseHS)
	STATE_MAP_ENTRY(&CMDServer::loopUplink)
	STATE_MAP_ENTRY(&CMDServer::loopDownlinkPrep)
	STATE_MAP_ENTRY(&CMDServer::loopDownlink)
	STATE_MAP_ENTRY(&CMDServer::loopPostPass)
	STATE_MAP_ENTRY(&CMDServer::loopReset)
	END_STATE_MAP

	enum CMD_States {
		ST_INIT = 0,
		ST_IDLE,
		ST_DIAGNOSTIC,
		ST_PASS_PREP,
		ST_LOGIN,
		ST_VERBOSE_HS,
		ST_UPLINK,
		ST_DOWNLINK_PREP,
		ST_DOWNLINK,
		ST_POST_PASS,
		ST_RESET,
	};
};

}
}

#endif /* CMDSERVER_H_ */
