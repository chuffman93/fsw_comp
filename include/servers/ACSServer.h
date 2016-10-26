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

typedef enum ACSPoint{
	ACS_UNORIENTED,
	ACS_SUN_ORIENTED,
	ACS_NADIR_ORIENTED,
	ACS_GND_ORIENTED,
	ACS_DEST_ORIENTED
}ACSOrientationType;

class ACSServer : public SubsystemServer, public AllStar::Core::Singleton{

	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

private:
	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	ACSOrientationType ACSOrientation;

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
	void loopPLDStart();
	void loopPLDPointing();
	void loopPLDStop();
	void loopCOMStart();
	void loopCOMPointing();
	void loopCOMStop();
	void loopDiagnostic();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&ACSServer::loopInit)
	STATE_MAP_ENTRY(&ACSServer::loopDisabled)
	STATE_MAP_ENTRY(&ACSServer::loopPLDStart)
	STATE_MAP_ENTRY(&ACSServer::loopPLDPointing)
	STATE_MAP_ENTRY(&ACSServer::loopPLDStop)
	STATE_MAP_ENTRY(&ACSServer::loopCOMStart)
	STATE_MAP_ENTRY(&ACSServer::loopCOMPointing)
	STATE_MAP_ENTRY(&ACSServer::loopCOMStop)
	STATE_MAP_ENTRY(&ACSServer::loopDiagnostic)
	END_STATE_MAP

	enum ACS_State{
		ST_INIT,
		ST_DISABLED,
		ST_PLD_START,
		ST_PLD_POINTING,
		ST_PLD_STOP,
		ST_COM_START,
		ST_COM_POINTING,
		ST_COM_STOP,
		ST_DIAGNOSTIC
	};

};

}
}

#endif /* ACSSERVER_H_ */
