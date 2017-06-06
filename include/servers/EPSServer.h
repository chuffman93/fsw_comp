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
#include "servers/Structs.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"

namespace AllStar{
namespace Servers{

class EPSServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	bool CheckHealthStatus();

	void bootConfig(); // read in configs on bootup
	bool updateConfig(); // update configs after COM pass

	static EPSStatus EPSState;
	static EPSConfig EPSConfiguration;

private:
	bool IsFullyInitialized(void);

	EPSServer(std::string nameIn, LocationIDType idIn);
	~EPSServer(){}
	EPSServer & operator=(const EPSServer & source);

	//Modes
	void loopInit();
	void loopMonitor();
	void loopReset();

	int32 lastHSTLog;

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
