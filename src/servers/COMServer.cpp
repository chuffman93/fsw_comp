/*
 * COMServer.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 */

#include "servers/COMServer.h"
#include "servers/COMHandlers.h"
#include "servers/COMStdTasks.h"
#include "servers/CDHServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// Instantiate static message handlers
static COMHSHandler * comHSHandler;

// -------------------------------------- Necessary Methods --------------------------------------
COMServer::COMServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), beaconData() {
}

COMServer::~COMServer(){
}

COMServer & COMServer::operator=(const COMServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void COMServer::Initialize(void){
	//comHSHandler = new COMHSHandler();
}

#ifdef TEST
void COMServer::Destroy(void){
	delete comHSHandler;
}
#endif

bool COMServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

bool COMServer::RegisterHandlers(){
	bool success = true;
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

//	// COM Command OpCodes
//	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_COM, COM_HS_CMD), comHSHandler);
//
//	// COM Command OpCodes permissions
//	for(int opcode = COM_CMD_MIN; opcode < COM_CMD_MAX; opcode++){
//		success &= arby.ModifyPermission(MessageIdentifierType(SERVER_LOCATION_COM, opcode), true);
//	}
//
//	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// TODO: ultimately remove Half and Full Duplex states and have a struct to track those COM states through H&S
// -------------------------------------------- State Machine ---------------------------------------------
void COMServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);

	usleep(5000000);

	if(COMTestAlive()){
		// Debug LED initialization
		if(!COMSelfCheck()){
			logger->Log(LOGGER_LEVEL_FATAL, "COM failed self check!");
		}
		logger->Log(LOGGER_LEVEL_INFO, "COM passed self check");

		currentState = ST_IDLE;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "COM non-responsive in init");
	}
}

void COMServer::loopIdle(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_EPS]){
		currentState = ST_INIT;
	}

	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_COM:
		currentState = ST_COM_START;
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

void COMServer::loopCOMStart(){
	if(COMHalfDuplex()){
		currentState = ST_COM_HALF;
	}else{
		currentState = ST_IDLE;
	}
}

void COMServer::loopCOMHalf(){
	if(COMFullDuplex()){
		currentState = ST_COM_FULL;
	}else{
		currentState = ST_IDLE;
	}
}

void COMServer::loopCOMFull(){
	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_EPS]){
		currentState = ST_INIT;
	}

	if(modeManager->GetMode() != MODE_COM){
		currentState = ST_COM_STOP;
	}
}

void COMServer::loopCOMStop(){
	COMSimplex();

	currentState = ST_IDLE;
}

void COMServer::loopDiagnostic(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_IDLE;
	}
}

void COMServer::loopReset(){
	COMPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_IDLE;
}

}
}


