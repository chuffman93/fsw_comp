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
#include "servers/CMDServer.h"
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
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), TXSilence(false) {
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
	return true;
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

		currentState = ST_BEACON;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "COM non-responsive in init");
	}
}

void COMServer::loopBeacon(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_EPS]){
		currentState = ST_INIT;
	}

	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}

	if(TXSilence){
		currentState = ST_ENTER_TX_SILENCE;
	}

	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	if(cmdServer->CheckForBeacon()){
		//COMSendBeacon();
	}
}

void COMServer::loopEnterTXSilence(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(!COMSimplex()){
		uint8 i = 0;
		while(i++ < 10){
			if(COMSimplex()){
				i = 20;
			}
		}
		if(i != 20){
			logger->Log(LOGGER_LEVEL_FATAL, "COM can't enter TX silence, serious problem");
		}
	}
	logger->Log(LOGGER_LEVEL_INFO, "COM entering TX silence");
	currentState = ST_TX_SILENCE;
}

void COMServer::loopTXSilence(){
	if(!TXSilence){
		currentState = ST_BEACON;
	}
}

void COMServer::loopReset(){
	COMPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_BEACON;
}

}
}


