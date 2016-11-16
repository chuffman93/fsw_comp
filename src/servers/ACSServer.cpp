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

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// -------------------------------------- Necessary Methods --------------------------------------
ACSServer::ACSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), ACSOrientation(ACS_UNORIENTED) {
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

}

#ifdef TEST
void ACSServer::Destroy(void){

}
#endif

bool ACSServer::IsFullyInitialized(void){
	return Singleton::IsFullyInitialized();
}

bool ACSServer::RegisterHandlers(){
	bool success = true;
	return success;
}

// -------------------------------------------- State Machine --------------------------------------------
void ACSServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);

	// delay while ACS boots up
	usleep(1000000);

	if(ACSTestAlive()){
		if(!ACSSelfCheck()){
			logger->Log("ACS failed self check!", LOGGER_LEVEL_FATAL);
		}

		logger->Log("ACS passed self check", LOGGER_LEVEL_INFO);

		currentState = ST_DISABLED;
	}else{
		logger->Log("ACS non-responsive in init", LOGGER_LEVEL_FATAL);
	}
}

void ACSServer::loopDisabled(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(ACSOrientation != ACS_SUN_ORIENTED){
		if(ACSPointSun()){
			ACSOrientation = ACS_SUN_ORIENTED;
		}
	}

	int64 wakeTime = getTimeInMillis();
	if(modeManager->GetMode() == MODE_COM)
		currentState = ST_COM_START;

	if(modeManager->GetMode() == MODE_PLD_PRIORITY)
		currentState = ST_PLD_START;

	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	ACSHealthStatus();

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}
	waitUntil(wakeTime,1000);
}

void ACSServer::loopPLDStart(){
	if(ACSPointNadir()){
		currentState = ST_PLD_POINTING;
		ACSOrientation = ACS_NADIR_ORIENTED;
	}else{
		currentState = ST_DISABLED;
	}
}

void ACSServer::loopPLDPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}

	int64 lastWake = getTimeInMillis();

	ACSHealthStatus();
	ACSSendGPS();

	waitUntil(lastWake, 1000);

	if(modeManager->GetMode() != MODE_PLD_PRIORITY)
		currentState = ST_PLD_STOP;
}

void ACSServer::loopPLDStop(){
	currentState = ST_DISABLED;
}

void ACSServer::loopCOMStart(){
	if(ACSPointGND()){
		currentState = ST_COM_POINTING;
		ACSOrientation = ACS_GND_ORIENTED;
	}else{
		currentState = ST_DISABLED;
	}
}

void ACSServer::loopCOMPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}

	int64 lastWake = getTimeInMillis();

	ACSHealthStatus();
	ACSSendGPS();

	waitUntil(lastWake, 5000);

	if(modeManager->GetMode() != MODE_COM)
		currentState = ST_COM_STOP;
}

void ACSServer::loopCOMStop(){
	currentState = ST_DISABLED;
}

void ACSServer::loopDiagnostic(){
	int64 lastWake = getTimeInMillis();

	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_DISABLED;
	}

	waitUntil(lastWake, 1000);
}

} // End Namespace servers
} // End Namespace AllStar
