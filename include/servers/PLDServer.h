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
#include "core/Dispatcher.h"
#include "core/ModeManager.h"

namespace AllStar{
namespace Servers{

struct PLDStatus{
	uint8 powerFault;
	uint16 motorSpeed;
	uint8 thermistors[10];
	uint8 acdDataWorking;
	uint16 control;
	uint8 byteSize;
};

class PLDServer : public SubsystemServer, public AllStar::Core::Singleton{

	friend class AllStar::Core::Factory;
public:
	bool RegisterHandlers();
	bool CheckHealthStatus();

	PLDStatus PLDState;

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	PLDServer(std::string nameIn, LocationIDType idIn);
	~PLDServer();
	PLDServer & operator=(const PLDServer & source); //possibly for firmware update

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
