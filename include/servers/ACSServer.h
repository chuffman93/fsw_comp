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
#include "core/ModeManager.h"
#include "core/Singleton.h"
#include "servers/Structs.h"

namespace AllStar {
namespace Servers {

#define ACS_SLEEP_TIME	5000
#define ACS_HS_DELAYS	3
#define ACS_CONFIG_SIZE	803		// this needs to be updated whenever the config is updated

typedef enum ACSModes {
	ACS_MODE_DETUMBLE = 0,
	ACS_MODE_SUN = 1,
	ACS_MODE_GND = 2,
	ACS_MODE_NADIR = 3,
	ACS_MODE_UNKNOWN = 10
} ACSModeType;

class ACSServer : public SubsystemServer, public AllStar::Core::Singleton {
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	bool CheckHealthStatus();

	void WriteMRPToFile();
	void BootMRPFromFile();

	void bootConfig(); // read in configs on bootup
	bool updateConfig(); // update configs after COM pass

	static ACSStatus ACSState;
	uint8 ACSConfig[ACS_CONFIG_SIZE];
	bool ACSConfigValid;

private:
	bool IsFullyInitialized(void);

	ACSServer(std::string nameIn, LocationIDType idIn);
	~ACSServer(void);
	ACSServer & operator=(const ACSServer & source);
	bool testsRun;
	int32 lastHSTLog;

	// ------ State Machine ----------------------------------------------------------
	void loopInit();
	void loopDetumble();
	void loopSunSoak();
	void loopPLDStart();
	void loopPLDPointing();
	void loopPLDStop();
	void loopCOMStart();
	void loopCOMPointing();
	void loopCOMStop();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&ACSServer::loopInit)
	STATE_MAP_ENTRY(&ACSServer::loopDetumble)
	STATE_MAP_ENTRY(&ACSServer::loopSunSoak)
	STATE_MAP_ENTRY(&ACSServer::loopPLDStart)
	STATE_MAP_ENTRY(&ACSServer::loopPLDPointing)
	STATE_MAP_ENTRY(&ACSServer::loopPLDStop)
	STATE_MAP_ENTRY(&ACSServer::loopCOMStart)
	STATE_MAP_ENTRY(&ACSServer::loopCOMPointing)
	STATE_MAP_ENTRY(&ACSServer::loopCOMStop)
	STATE_MAP_ENTRY(&ACSServer::loopReset)
	END_STATE_MAP

	enum ACS_State {
		ST_INIT,
		ST_DETUMBLE,
		ST_SUN_SOAK,
		ST_PLD_START,
		ST_PLD_POINTING,
		ST_PLD_STOP,
		ST_COM_START,
		ST_COM_POINTING,
		ST_COM_STOP,
		ST_RESET
	};
};

}
}

#endif /* ACSSERVER_H_ */
