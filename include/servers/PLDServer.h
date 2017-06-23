/*
 * PayloadServer.h
 *
 *  Created on: May 27, 2011
 *      Author: Michael
 * Last Updated by: Robert Belter
 *
 */

#ifndef PLDSERVER_H_
#define PLDSERVER_H_

#include "core/StdTypes.h"
#include "servers/SubsystemServer.h"
#include "servers/Structs.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"

#define RAD_CHUNK_SIZE		102400 // 100 kB

namespace AllStar{
namespace Servers{

class PLDServer : public SubsystemServer, public AllStar::Core::Singleton{

	friend class AllStar::Core::Factory;
public:
	bool RegisterHandlers();
	bool CheckHealthStatus();

	void bootConfig(); // read in configs on bootup
	bool updateConfig(); // update configs after COM pass

	static PLDConfig PLDConfiguration;

	PLDStatus PLDState;

	static uint16 PLDDataNumber;

private:
	bool IsFullyInitialized(void);

	PLDServer(std::string nameIn, LocationIDType idIn);
	~PLDServer();
	PLDServer & operator=(const PLDServer & source); //possibly for firmware update

	int tftp_pid;
	char dataFile[100];
	int32 lastHSTLog;

	void loopInit();
	void loopIdle();
	void loopStartup();
	void loopShutdown();
	void loopScience();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&PLDServer::loopInit)
	STATE_MAP_ENTRY(&PLDServer::loopIdle)
	STATE_MAP_ENTRY(&PLDServer::loopStartup)
	STATE_MAP_ENTRY(&PLDServer::loopScience)
	STATE_MAP_ENTRY(&PLDServer::loopShutdown)
	STATE_MAP_ENTRY(&PLDServer::loopReset)
	END_STATE_MAP

	enum PLD_States {
		ST_INIT = 0,
		ST_IDLE,
		ST_STARTUP,
		ST_SCIENCE,
		ST_SHUTDOWN,
		ST_RESET
	};
};

}
}

#endif // PLDSERVER_H_
