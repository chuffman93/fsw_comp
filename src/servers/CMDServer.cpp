/*
* CMDServer.cpp
*
*  Created on: Feb 12, 2013
*      Author: Caitlyn
*  Modified by:
*/

#include "servers/CMDHandlers.h"
#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "POSIX.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <sys/inotify.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

//Message Handler for ACP Protocol Switches
static CMDSwitchProtocolHandler * cmdSwitchProtocolHandler;
int CMDServer::subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

CMDServer::CMDServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), passStart(0){
	for (int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++) {
		subsystem_acp_protocol[i] = ACP_PROTOCOL_SPI;
	}
}

CMDServer::~CMDServer(){
}

void CMDServer::Initialize(void){
	cmdSwitchProtocolHandler = new CMDSwitchProtocolHandler();
}

#ifdef TEST
void CMDServer::Destroy(void){
	delete cmdSwitchProtocolHandler;
}
#endif

bool CMDServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

CMDServer & CMDServer::operator=(const CMDServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool CMDServer::RegisterHandlers(){
	bool success = true;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	// CMD Command Opcode
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CMD, CMD_ACP_SWITCH), cmdSwitchProtocolHandler);
	success &= arby.ModifyPermission(MessageIdentifierType(SERVER_LOCATION_CMD, CMD_ACP_SWITCH), true);

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// --------- State Machine -----------------------------------------------------------------------------------------
void CMDServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: Initializing");

	// setup for uftp
	portSetup();
	uftpSetup();

	currentState = ST_IDLE;
}

void CMDServer::loopIdle(void){
	int64 LastTimeTick = getTimeInMillis();
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	if(modeManager->GetMode() == MODE_COM){
		currentState = ST_PRE_PASS;
	}

	dispatcher->CleanRXQueue();

	waitUntil(LastTimeTick, 1000);
}

void CMDServer::loopDiagnostic(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	runDiagnostic();

	currentState = ST_IDLE;
	modeManager->SetMode(MODE_BUS_PRIORITY);
}

void CMDServer::loopPrePass(){
	/* TODO:
	 * Tell the FMG server to go to its COM state
	 * Command ACS to point to the ground station
	 * Gather Verbose H&S files
	 */

	currentState = ST_LOGIN;
}

// TODO: determine login sequence
void CMDServer::loopLogin(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// if(login){currState = Verbose}
	// else{retry until COM over? continue to beacon}

	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: ground login successful");
	passStart = getTimeInSec();
	currentState = ST_VERBOSE_HS;
}

void CMDServer::loopVerboseHS(){
	// downlink the Verbose H&S file
	currentState = ST_UPLINK;
}

void CMDServer::loopUplink(){
	// Uplink ends after the post pass execution script has been received
	currentState = ST_DOWNLINK;
}

void CMDServer::loopDownlink(){
	// fork a new process to downlink files
	currentState = ST_POST_PASS;
}

void CMDServer::loopPostPass(){
	parsePPE();

	// Clear downlink directory
	string cmd = "rm -rf" + string(DOWNLINK_DIRECTORY) + "/*";
	//system(cmd.c_str());

	parseDRF();
	parseDLT();
	processUplinkFiles();

	// Clear uplink directory
	cmd = "rm -rf " + string(UPLINK_DIRECTORY) + "/*";
	//system(cmd.c_str());

	// TODO: switch FMG server out of COM mode

	currentState = ST_IDLE;
}

} // End of namespace Servers
} // End of namespace AllStar
