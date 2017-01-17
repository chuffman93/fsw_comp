/*
 * PayloadServer.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Michael
 *
 *  Last Updated by: Robert Belter
 */

#include "servers/PLDServer.h"
#include "servers/PLDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "servers/CDHServer.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// Message handler for a health and status command.
static PLDHSHandler * pldHSHandler;

PLDServer::PLDServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn) {
}

PLDServer::~PLDServer(){
}

PLDServer & PLDServer::operator=(const PLDServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void PLDServer::Initialize(void){
	pldHSHandler = new PLDHSHandler();
}

#ifdef TEST
void PLDServer::Destroy(void){
	delete pldHSHandler;
}
#endif

bool PLDServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

bool PLDServer::RegisterHandlers(){
	bool success = true;
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_PLD, HEALTH_STATUS_CMD), pldHSHandler);

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}


// ------------------------------------ State Machine ------------------------------------------------------------
void PLDServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "PLDServer: initializing");

	// TODO: synchronize clock with GPS

	//No need to do anything
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_STARTUP){
		//Wait until we are not in startup mode, then transition to IDLE
		currentState = ST_IDLE;
	}
}

void PLDServer::loopIdle(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_PLD_PRIORITY:
		currentState = ST_STARTUP;
		break;
	case MODE_DIAGNOSTIC:
		currentState = ST_DIAGNOSTIC;
		break;
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}
}

void PLDServer::loopStartup(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->resetAssert(HARDWARE_LOCATION_PLD);
	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
	usleep(300000);
	cdhServer->resetDeassert(HARDWARE_LOCATION_PLD);

	usleep(1000000);

	ACPPacket * turnOnScience = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_START_SCIENCE);
	ACPPacket * ret = DispatchPacket(turnOnScience);
	if(ret->getOpcode() != PLD_START_SCIENCE){
		//TODO Error handling
		currentState = ST_IDLE;
	}
	delete ret;
	//PLD is on and in science mode. Goto science loop
	currentState = ST_SCIENCE;
}

// Graceful shutdown
void PLDServer::loopShutdown(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	ACPPacket * turnOffScience = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_STOP_SCIENCE);
	ACPPacket * ret = DispatchPacket(turnOffScience);
	delete ret;

	usleep(1000000);

	cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
	//PLD is off. Goto idle loop
	currentState = ST_IDLE;

}
void PLDServer::loopScience(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// Make sure PLD hasn't been turned off due to a fault, if it has, go back into startup
	if(cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD] == false){
		currentState = ST_STARTUP;
	}

	// Stay in science mode until we move out of PLD_PRIORITY
	if(modeManager->GetMode() != MODE_PLD_PRIORITY){
		currentState = ST_SHUTDOWN;
	}
}

void PLDServer::loopDiagnostic(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_IDLE;
	}
}

void PLDServer::loopReset(){
	// make sure payload is off?
	usleep(100000);
}

} // end namespace servers
} // end namespace AllStar
