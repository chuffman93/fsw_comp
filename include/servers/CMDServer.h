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

#define DOWNLINK_DIRECTORY			"/home/root/fileSysTest/home/downlink/"
#define UPLINK_DIRECTORY			"/home/root/fileSysTest/home/uplink/"
#define FILES_TO_DOWNLINK_UPLK		"/home/root/fileSysTest/home/uplink/f2d"
#define POST_PASS_EXECUTION_UPLK	"/home/root/fileSysTest/home/uplink/pp"
#define IMMEDIATE_EXECUTION_UPLK 	"/home/root/fileSysTest/home/uplink/im"
#define SCHEDULE_UPLK				"/home/root/fileSysTest/home/uplink/schedule"

// Define a struct type for the downlink priority queue, needs overloaded "<" operator to handle priority
struct downlinkFile{
	string fileName;
	string pathName;
	uint8 priority;

	bool operator<(const downlinkFile& file2) const{
		return(priority < file2.priority);
	}
};

class CMDServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	static int subsystem_acp_protocol[HARDWARE_LOCATION_MAX];
	priority_queue<downlinkFile> downlinkPriorityQueue;

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

	// Modes
	void loopInit();
	void loopIdle();
	void loopDiagnostic();
	void loopLogin();
	void loopVerboseHS();
	void loopUplink();
	void loopDownlink();
	void loopQueueEmpty();
	void loopPostPass();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&CMDServer::loopInit)
	STATE_MAP_ENTRY(&CMDServer::loopIdle)
	STATE_MAP_ENTRY(&CMDServer::loopDiagnostic)
	STATE_MAP_ENTRY(&CMDServer::loopLogin)
	STATE_MAP_ENTRY(&CMDServer::loopVerboseHS)
	STATE_MAP_ENTRY(&CMDServer::loopUplink)
	STATE_MAP_ENTRY(&CMDServer::loopDownlink)
	STATE_MAP_ENTRY(&CMDServer::loopQueueEmpty)
	STATE_MAP_ENTRY(&CMDServer::loopPostPass)
	END_STATE_MAP

	enum CMD_States {
		ST_INIT = 0,
		ST_IDLE,
		ST_DIAGNOSTIC,
		ST_LOGIN,
		ST_VERBOSE_HS,
		ST_UPLINK,
		ST_DOWNLINK,
		ST_QUEUE_EMPTY,
		ST_POST_PASS
	};
};

}
}

#endif /* CMDSERVER_H_ */
