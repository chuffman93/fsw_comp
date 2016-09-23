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
	comHSHandler = new COMHSHandler();
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

	// COM Command OpCodes
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_COM, COM_HS_CMD), comHSHandler);

	// COM Command OpCodes permissions
	for(int opcode = COM_CMD_MIN; opcode < COM_CMD_MAX; opcode++){
		success &= arby.ModifyPermission(MessageIdentifierType(SERVER_LOCATION_COM, opcode), true);
	}

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- State Machine ---------------------------------------------
void COMServer::loopInit(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);

	usleep(5000000);

	currentState = ST_IDLE;
}

void COMServer::loopIdle(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_EPS]){
		currentState = ST_INIT;
	}

	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if(modeManager->GetMode() == MODE_COM){
		currentState = ST_COM_START;
	}
}

void COMServer::loopCOMStart(){
	ACPPacket * startPacket = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 3);
	ACPPacket * startRet = DispatchPacket(startPacket);

	currentState = ST_COM;
}

void COMServer::loopCOM(){
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
	ACPPacket * stopPacket = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 4);
	ACPPacket * stopRet = DispatchPacket(stopPacket);

	currentState = ST_IDLE;
}

}
}


