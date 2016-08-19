/*
 * ACSServer.h
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 *
 *  Updated: Alex St. Clair
 */

#ifndef ACSSERVER_H_
#define ACSSERVER_H_

#include "core/StdTypes.h"
#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "servers/ACSHandlers.h"
#include "core/ModeManager.h"
#include "core/Singleton.h"

namespace AllStar{
namespace Servers{

class ACSServer : public SubsystemServer, public AllStar::Core::Singleton{

	friend class AllStar::Core::Factory;

public:
	void Update(SystemModeEnum mode){}
	bool RegisterHandlers();

private:
	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	ACSServer(std::string nameIn, LocationIDType idIn);
	~ACSServer(void);
	ACSServer & operator=(const ACSServer & source);


	// ------ State Machine ----------------------------------------------------------
	void loopInit();
	void loopDisabled();
	void loopGNDStart();
	void loopGNDPointing();
	void loopGNDStop();
	void loopPLDStart();
	void loopPLDPointing();
	void loopPLDStop();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&ACSServer::loopInit)
	STATE_MAP_ENTRY(&ACSServer::loopDisabled)
	STATE_MAP_ENTRY(&ACSServer::loopGNDStart)
	STATE_MAP_ENTRY(&ACSServer::loopGNDPointing)
	STATE_MAP_ENTRY(&ACSServer::loopGNDStop)
	STATE_MAP_ENTRY(&ACSServer::loopPLDStart)
	STATE_MAP_ENTRY(&ACSServer::loopPLDPointing)
	STATE_MAP_ENTRY(&ACSServer::loopPLDStop)
	END_STATE_MAP

	enum ACS_State{
		ST_INIT,
		ST_DISABLED,
		ST_GND_START,
		ST_GND_POINTING,
		ST_GND_STOP,
		ST_PLD_START,
		ST_PLD_POINTING,
		ST_PLD_STOP
	};

};

}
}

#endif /* ACSSERVER_H_ */
