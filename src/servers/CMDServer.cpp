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
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), numFilesDWN(0), currFileNum(0) {
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
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	if(modeManager->GetMode() == MODE_COM){
		currentState = ST_PRE_PASS;
	}

	dispatcher->CleanRXQueue();
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
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	bool loggedIn = true;

	if(loggedIn){
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: ground login successful");
		currentState = ST_VERBOSE_HS;
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: login unsuccessful, COM pass over");
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopVerboseHS(){
	// downlink the Verbose H&S file
	currentState = ST_UPLINK;
}

void CMDServer::loopUplink(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// check if the EOT file has been uplinked
	if(access(EOT_PATH,F_OK) == 0){
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: uplink concluded");
		currentState = ST_DOWNLINK_PREP;
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: login unsuccessful, COM pass over");
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopDownlinkPrep(){
	numFilesDWN = getNumFiles((char *) DOWNLINK_DIRECTORY);
	currFileNum = 0;
	currentState = ST_DOWNLINK;
}

void CMDServer::loopDownlink(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	string filename;

	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: downlink timed out, COM pass over");
		numFilesDWN = 0;
		currFileNum = 0;
		currentState = ST_POST_PASS;
	}

	if(currFileNum < numFilesDWN){
		filename = getDownlinkFile(currFileNum++);
		if(strcmp(filename.c_str(),"") == 0){
			// downlink the file
			printf("File: %s\n", filename.c_str());
		}
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: downlink finished");
		numFilesDWN = 0;
		currFileNum = 0;
		currentState = ST_POST_PASS;
	}

}

void CMDServer::loopPostPass(){
	parsePPE();

	// Clear downlink directory
	string cmd = "rm -rf" + string(DOWNLINK_DIRECTORY) + "/*";
	//system(cmd.c_str());

	parseDLT();
	parseDRF();
	processUplinkFiles();

	// Clear uplink directory
	cmd = "rm -rf " + string(UPLINK_DIRECTORY) + "/*";
	//system(cmd.c_str());

	// TODO: switch FMG server out of COM mode

	currentState = ST_IDLE;
}

} // End of namespace Servers
} // End of namespace AllStar
