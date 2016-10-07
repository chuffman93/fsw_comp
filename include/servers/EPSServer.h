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
#include "servers/EPSHandlers.h"
#include "core/StdTypes.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "util/FileHandler.h"

namespace AllStar{
namespace Servers{

class EPSServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

private:

	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	EPSServer(std::string nameIn, LocationIDType idIn);
	~EPSServer(){}
	EPSServer & operator=(const EPSServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	//Modes
	void loopInit();
	void loopMonitor();
	void loopDiagnostic();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&EPSServer::loopInit)
	STATE_MAP_ENTRY(&EPSServer::loopMonitor)
	STATE_MAP_ENTRY(&EPSServer::loopDiagnostic)
	END_STATE_MAP

	enum EPS_States{
		ST_INIT = 0,
		ST_MONITOR,
		ST_DIAGNOSTIC
	};

};

}
}

#endif /* EPSSERVER_H_ */
