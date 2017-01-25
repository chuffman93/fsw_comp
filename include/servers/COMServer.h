/*
 * COMServer.h
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 *      Modified: Brian Campuzano
 */

#ifndef COMSERVER_H_
#define COMSERVER_H_

#include "servers/SubsystemServer.h"
#include "servers/COMHandlers.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/Factory.h"

namespace AllStar{
namespace Servers{

class COMServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers(void);

	bool checkTxSilence() const {
		return TXSilence;
	}

	void setTxSilence(bool txSilence) {
		TXSilence = txSilence;
	}

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	COMServer(std::string nameIn, LocationIDType idIn);
	~COMServer(void);
	COMServer & operator=(const COMServer & source);

	// Member variables for handler registration
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	bool TXSilence;

	// ------ State Machine ---------------------------------------------------
	void loopInit();
	void loopBeacon();
	void loopEnterTXSilence();
	void loopTXSilence();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&COMServer::loopInit)
	STATE_MAP_ENTRY(&COMServer::loopBeacon)
	STATE_MAP_ENTRY(&COMServer::loopEnterTXSilence)
	STATE_MAP_ENTRY(&COMServer::loopTXSilence)
	STATE_MAP_ENTRY(&COMServer::loopReset)
	END_STATE_MAP

	enum COM_States {
		ST_INIT = 0,
		ST_BEACON,
		ST_ENTER_TX_SILENCE,
		ST_TX_SILENCE,
		ST_RESET
	};
};

}
}

#endif /* COMSERVER_H_ */
