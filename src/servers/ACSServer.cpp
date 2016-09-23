/*
* ACSServer.cpp
*
*  Created on: Feb 21, 2011
*      Author: Michael
*  Modified by: Conrad Hougen
*  Updated: Alex St. Clair
*/

#include "servers/ACSStdTasks.h"
#include "servers/ACSServer.h"
#include "servers/ACSHandlers.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/Dispatcher.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#define HARDWARE

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

static ACSMeasurementHandler * acsHSHandler;
static ACSGPSHandler * acsGPSHandler;

// -------------------------------------- Necessary Methods --------------------------------------
ACSServer::ACSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn) {
}

ACSServer::~ACSServer() {
}

ACSServer & ACSServer::operator=(const ACSServer & source){
	if (this == &source)
	{
		return *this;
	}

	SubsystemServer::operator =(source);
	return *this;
}

void ACSServer::Initialize(void){
	//Initialize handlers
	acsHSHandler = new ACSMeasurementHandler();
	acsGPSHandler = new ACSGPSHandler();
}

#ifdef TEST
void ACSServer::Destroy(void){
	//delete handlers
	delete acsHSHandler;
	delete acsGPSHandler;
}
#endif

bool ACSServer::IsFullyInitialized(void){
	return Singleton::IsFullyInitialized();
}

bool ACSServer::RegisterHandlers(){
	bool success = true;
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	// ACS Command OpCodes
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_ACS, ACS_HS_CMD), acsHSHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_ACS, ACS_GPS_CMD), acsGPSHandler);

	for(int opcode = ACS_CMD_MIN; opcode < ACS_CMD_MAX; opcode++){
		success &= arby.ModifyPermission(MessageIdentifierType(SERVER_LOCATION_ACS, opcode), true);
	}

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- State Machine --------------------------------------------
void ACSServer::loopInit(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
	currentState = ST_DISABLED;
}

void ACSServer::loopDisabled(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(modeManager->GetMode() == MODE_COM)
		currentState = ST_GND_START;

	if(modeManager->GetMode() == MODE_PLD_PRIORITY)
		currentState = ST_PLD_START;

	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}
}

void ACSServer::loopGNDStart(){
	currentState = ST_GND_POINTING;
}

void ACSServer::loopGNDPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}

	if(modeManager->GetMode() != MODE_COM)
		currentState = ST_GND_STOP;
}

void ACSServer::loopGNDStop(){
	currentState = ST_DISABLED;
}

void ACSServer::loopPLDStart(){
	currentState = ST_PLD_POINTING;
}
void ACSServer::loopPLDPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	ACPPacket * GPSRet;
	ACPPacket * HSRet;

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}

	GPSRet = ACSSendGPS();
	//PacketProcess(SERVER_LOCATION_ACS, GPSRet);

	usleep(3000000);

	ACPPacket * HSSend = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_HS_CMD);
	HSRet = DispatchPacket(HSSend);
	//PacketProcess(SERVER_LOCATION_ACS, HSRet);

	if(modeManager->GetMode() != MODE_PLD_PRIORITY)
		currentState = ST_PLD_STOP;
}

void ACSServer::loopPLDStop(){
	currentState = ST_DISABLED;
}

void ACSServer::loopDiagnostic(){
	uint64 lastWake = getTimeInMillis();

	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_DISABLED;
	}

	waitUntil(lastWake, 1000);
}

} // End Namespace servers
} // End Namespace AllStar
