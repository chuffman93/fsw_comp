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

#define DOWNLINK_DIRECTORY	"/home/root/fileSysTest/home/downlink"
#define UPLINK_DIRECTORY	"/home/root/fileSysTest/home/uplink"
#define DRF_PATH			"/home/root/fileSysTest/home/uplink/drf"
#define DLT_PATH			"/home/root/fileSysTest/home/uplink/dlt"
#define PPE_PATH			"/home/root/fileSysTest/home/uplink/ppe"
#define SCHEDULE_PATH		"/home/root/fileSysTest/home/uplink/schedule"

#define PASS_TIMEOUT		900		// 900 s = 15 min
#define FILE_CHUNK_SIZE		"500k"

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

	// COM Pass Variables
	int32 passStart;

	// Modes
	void loopInit();
	void loopIdle();
	void loopDiagnostic();
	void loopPrePass();
	void loopLogin();
	void loopVerboseHS();
	void loopUplink();
	void loopDownlink();
	void loopPostPass();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&CMDServer::loopInit)
	STATE_MAP_ENTRY(&CMDServer::loopIdle)
	STATE_MAP_ENTRY(&CMDServer::loopDiagnostic)
	STATE_MAP_ENTRY(&CMDServer::loopPrePass)
	STATE_MAP_ENTRY(&CMDServer::loopLogin)
	STATE_MAP_ENTRY(&CMDServer::loopVerboseHS)
	STATE_MAP_ENTRY(&CMDServer::loopUplink)
	STATE_MAP_ENTRY(&CMDServer::loopDownlink)
	STATE_MAP_ENTRY(&CMDServer::loopPostPass)
	END_STATE_MAP

	enum CMD_States {
		ST_INIT = 0,
		ST_IDLE,
		ST_DIAGNOSTIC,
		ST_PRE_PASS,
		ST_LOGIN,
		ST_VERBOSE_HS,
		ST_UPLINK,
		ST_DOWNLINK,
		ST_POST_PASS
	};
};

}
}

#endif /* CMDSERVER_H_ */
