/*
 * SubsystemServer.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 *
 *  Last Modified: Jack Dinkel 12/16/16
 *
 *   State Machine template from : http://www.drdobbs.com/cpp/state-machine-design-in-c/184401236?pgno=2
 */

#ifndef SUBSYSTEMSERVER_H_
#define SUBSYSTEMSERVER_H_

#include "servers/Server.h"
#include "core/StdTypes.h"
#include <stdint.h>

namespace AllStar {
namespace Servers {

struct StateStruct;
class SubsystemServer : public Server {
public:
	SubsystemServer(std::string nameIn, LocationIDType idIn);

	SubsystemServer(std::string nameIn, LocationIDType idIn, int sleep, int delays);

	virtual ~SubsystemServer();

	SubsystemServer & operator=(const SubsystemServer & source);

	virtual bool operator ==(const Server & check) const;

	virtual void SubsystemLoop(void);

	inline void wdmAlive(void) {
		this->wdmState = WDM_ALIVE;
	}

	inline void wdmAsleep(void) {
		this->wdmState = WDM_ASLEEP;
	}

	inline void wdmUnknown(void) {
		this->wdmState = WDM_UNKNOWN;
	}

	inline WDMStateType getWDMState(void) {
		return wdmState;
	}

protected:
	WDMStateType wdmState;

	uint16_t currentState;

	int sleepTime;

	int hsDelays;

	virtual const StateStruct * GetStateMap() = 0;

	virtual bool CheckHealthStatus(void);
};

typedef void (SubsystemServer::*StateFunc)(void);
struct StateStruct{
	StateFunc function;
};

#define BEGIN_STATE_MAP \
	public:\
	const StateStruct* GetStateMap() {\
		static const StateStruct StateMap[] = {

#define STATE_MAP_ENTRY(entry)\
		{reinterpret_cast<StateFunc>(entry)},

#define END_STATE_MAP \
	};\
	return &StateMap[0];}

} // End of namespace Servers
} // End of namespace AllStar

#endif /* SUBSYSTEMSERVER_H_ */
