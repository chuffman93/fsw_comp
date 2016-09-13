/*
 * EPSServer.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 *  Modified by: Conrad Hougen
 *  Modified by: Alex St. Clair
 */

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "servers/EPSHandlers.h"
#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// Instantiate static message handlers
static EPSHSHandler * epsHSHandler;
static EPSPowerCycleHandler * epsPowerCycleHandler;

// -------------------------------------- Necessary Methods --------------------------------------
EPSServer::EPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn) {
}

EPSServer & EPSServer::operator=(const EPSServer & source){
	if(this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void EPSServer::Initialize(void){
	//Initialize handlers
	epsHSHandler = new EPSHSHandler();
	epsPowerCycleHandler = new EPSPowerCycleHandler();
}

#ifdef TEST
void EPSServer::Destroy(void){
	//delete handlers
	delete epsHSHandler;
	delete epsPowerCycleHandler;
}
#endif

bool EPSServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

bool EPSServer::RegisterHandlers() {
	bool success = true;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *>(Factory::GetInstance(DISPATCHER_SINGLETON));

	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_EPS, EPS_HS_CMD),epsHSHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_EPS, EPS_POWER_CYCLE_CMD),epsPowerCycleHandler);

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- State Machine ---------------------------------------------
void EPSServer::loopInit(){
	currentState = ST_MONITOR;
}

void EPSServer::loopMonitor(){
	ACPPacket * HSRet = EPSHealthStat();
	PacketProcess(SERVER_LOCATION_EPS, HSRet);
	usleep(1000000);
}

} // End Namespace servers
} // End Namespace AllStar
