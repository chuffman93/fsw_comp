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
#include "servers/SCHServer.h"
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
	uftpSetup();

	currentState = ST_IDLE;
}

void CMDServer::loopIdle(void){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	if(modeManager->GetMode() == MODE_COM){
		currentState = ST_PASS_PREP;
	}

	if(access(SOT_PATH, F_OK) == 0){
		logger->Log(LOGGER_LEVEL_WARN, "CMDServer: unexpected ground login, preparing to enter COM Mode");
		if(modeManager->GetMode() == MODE_BUS_PRIORITY){
			SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
			schServer->RequestCOMMode();
		}
	}

	dispatcher->CleanRXQueue();
}

void CMDServer::loopDiagnostic(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	runDiagnostic();

	currentState = ST_IDLE;
	modeManager->SetMode(MODE_BUS_PRIORITY);
}

void CMDServer::loopPassPrep(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	/* TODO:
	 * Tell the FMG server to go to its COM state
	 * Command ACS to point to the ground station
	 * Gather Verbose H&S files
	 */

	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: finished COM pass prep");
	currentState = ST_LOGIN;
}

// TODO: determine login sequence
void CMDServer::loopLogin(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// block until the start of transmission file has been uplinked
	if(access(SOT_PATH,F_OK) == 0){
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// downlink the Verbose H&S file
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: finished downlinking Verbose H&S");
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
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: uplink not finished, COM pass over");
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopDownlinkPrep(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	numFilesDWN = getNumFiles((char *) DOWNLINK_DIRECTORY);
	currFileNum = 1;

	logger->Log(LOGGER_LEVEL_INFO,"CMDServer: %d files to downlink", numFilesDWN);
	currentState = ST_DOWNLINK;
}

void CMDServer::loopDownlink(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	string filename;

	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: downlink timed out, COM pass over");
		numFilesDWN = 0;
		currFileNum = 1;
		currentState = ST_POST_PASS;
	}

	if(currFileNum < numFilesDWN + 1){
		filename = getDownlinkFile(currFileNum++);

		// get downlink returns an empty string if it errors, check this before we downlink the file
		if(strcmp(filename.c_str(),"") != 0){
			// downlink the file
			char sh_cmd[256];
			sprintf(sh_cmd, "/home/root/uftp -I ax0 -H 1.1.1.2 -x 1 %s", filename.c_str()); // can add "-H 1.1.1.2" to only downlink to one IP, "-x 1" decreases the log statement verboseness
			system(sh_cmd);
		}
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: downlink finished");
		numFilesDWN = 0;
		currFileNum = 1;
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopPostPass(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	parsePPE();

	// Clear downlink directory
	string cmd = "rm -rf " + string(DOWNLINK_DIRECTORY) + "/*";
	system(cmd.c_str());

	parseDLT();
	parseDRF();
	processUplinkFiles();

	// Clear uplink directory
	cmd = "rm -rf " + string(UPLINK_DIRECTORY) + "/*";
	system(cmd.c_str());

	// TODO: switch FMG server out of COM mode

	if(modeManager->GetMode() == MODE_COM){
		modeManager->SetMode(MODE_BUS_PRIORITY);
	}
	currentState = ST_IDLE;
}

} // End of namespace Servers
} // End of namespace AllStar
