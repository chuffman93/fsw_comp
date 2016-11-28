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
	EPSState = {0,0,0,0,0,0,0,0};
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
//	epsHSHandler = new EPSHSHandler();
//	epsPowerCycleHandler = new EPSPowerCycleHandler();
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

	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_EPS, HEALTH_STATUS_CMD),epsHSHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_EPS, SUBSYSTEM_RESET_CMD),epsPowerCycleHandler);

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- State Machine ---------------------------------------------
void EPSServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(EPSTestAlive()){
		if(!EPSSelfCheck()){
			logger->Log(LOGGER_LEVEL_FATAL, "EPS failed self check!");
		}
		logger->Log(LOGGER_LEVEL_INFO, "EPS passed self check");

		currentState = ST_MONITOR;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "EPS non-responsive in init loop");
		usleep(3000000);
	}
}

void EPSServer::loopMonitor(){
	EPSHealthStat();

	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	usleep(2000000);
}

void EPSServer::loopDiagnostic(){
	int64 lastWake = getTimeInMillis();

	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_MONITOR;
	}

	waitUntil(lastWake, 1000);
}

} // End Namespace servers
} // End Namespace AllStar
