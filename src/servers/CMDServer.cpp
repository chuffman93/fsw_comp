/*
* CMDServer.cpp
*
*  Created on: Feb 12, 2013
*      Author: Caitlyn
*  Modified by: Alex, January 2017
*/

#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/SCHServer.h"
#include "servers/FMGServer.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/CDHServer.h"
#include "servers/PLDServer.h"
#include "servers/FileSystem.h"
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
int CMDServer::subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

CMDConfig CMDServer::CMDConfiguration(360,1000,1000,10,10);

CMDServer::CMDServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), numFilesDWN(0), currFileNum(0) {
	for (int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++) {
		subsystem_acp_protocol[i] = ACP_PROTOCOL_SPI;
	}

	startTime = getTimeInSec();
	CMDConfiguration.resetPeriod = 24*60*60; // 24 hrs * 60 min/hr * 60 sec/min
	CMDConfiguration.fileChunkSize = 10240; // 10KB
	CMDConfiguration.maxDownlinkSize = 15728640;
	CMDConfiguration.beaconPeriod = 15;
	CMDConfiguration.increasedBeaconPeriod = 8;
}

CMDServer::~CMDServer(){
}

void CMDServer::Initialize(void){
}

#ifdef TEST
void CMDServer::Destroy(void){
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

// --------- State Machine -----------------------------------------------------------------------------------------
void CMDServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: Initializing");

	hsDelays = CMDConfiguration.beaconPeriod;

	// setup for uftp
	uftpSetup();
	bootConfig();

	currentState = ST_IDLE;
}

void CMDServer::loopIdle(void){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// check for state transitions from mode switches
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_COM:
		currentState = ST_PASS_PREP;
		return;
	case MODE_RESET:
		currentState = ST_RESET;
		return;
	default:
		break;
	}

	// Check for ground login
	if(checkForSOT() != 0){
		logger->Log(LOGGER_LEVEL_WARN, "CMDServer: surprise COM! Preparing to enter COM Mode");
		SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
		schServer->RequestCOMMode();
		int64 currTime = getTimeInMillis();
		waitUntil(currTime,5000);
		currentState = ST_PASS_PREP;
		return;
	}

	// Monitor daily reset
	if(getTimeInSec() > (startTime + CMDConfiguration.resetPeriod) && (modeManager->GetMode() == MODE_BUS_PRIORITY)){
		SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
		schServer->RequestReset();
		currentState = ST_RESET;
		return;
	}

	dispatcher->CleanRXQueue();
}

void CMDServer::loopPassPrep(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	hsDelays = CMDConfiguration.increasedBeaconPeriod; // up the beacon rate

	// see if the fmgServer has prepped Verbose H&S
	if(fmgServer->isComReady()){
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: finished COM pass prep");
		currentState = ST_LOGIN;
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: FMGServer never prepped for COM, COM pass over");
		hsDelays = CMDConfiguration.beaconPeriod; // reset beacon period
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopLogin(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// block until the start of transmission file has been uplinked
	int SOTresult = checkForSOT();
	if(SOTresult == 1){
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: ground login successful, skipping VH&S");
		hsDelays = CMDConfiguration.beaconPeriod; // set the beacon to normal rate
		currentState = ST_UPLINK;
	} else if(SOTresult == 2){
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: ground login successful, sending VH&S");
		hsDelays = CMDConfiguration.beaconPeriod; // set the beacon to normal rate
		currentState = ST_VERBOSE_HS;
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: no login, COM pass over");
		hsDelays = CMDConfiguration.beaconPeriod; // set the beacon to normal rate
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopVerboseHS(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// downlink the Verbose H&S file
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: Verbose H&S unimplemented");
	currentState = ST_UPLINK;
}

void CMDServer::loopUplink(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// check if the EOT file has been uplinked
	if(access(EOT_PATH,F_OK) == 0) {
		// if EOT, check for IEF
		if(access(IEF_PATH,F_OK) == 0) {
			logger->Log(LOGGER_LEVEL_INFO, "CMDServer: uplink concluded, beginning immediate execution");
			currentState = ST_IMMEDIATE_EXECUTION;
		} else {
			logger->Log(LOGGER_LEVEL_INFO, "CMDServer: uplink concluded, starting planned downlink");
			currentState = ST_DOWNLINK_PREP;
		}
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: uplink not finished, COM pass over");
		currentState = ST_POST_PASS;
	}
}

void CMDServer::loopImmediateExecution() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: starting immediate execution");

	parseIEF();

	// Clear immed directory
	system("rm -rf " IMMED_DIRECTORY "/*");

	// Clear uplink directory
	system("rm -rf " UPLINK_DIRECTORY "/*");

	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_COM){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: uplink not finished, COM pass over");
		currentState = ST_POST_PASS;
	}

	currentState = ST_UPLINK;
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

		// downlink the file
		wdmAsleep();
		downlinkFile(filename);
		wdmAlive();
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
	system("rm -rf " DOWNLINK_DIRECTORY "/*");

	parseDLT();
	parseDRF();
	processUplinkFiles();

	// Clear uplink directory
	system("rm -rf " UPLINK_DIRECTORY "/*");

	if(modeManager->GetMode() == MODE_COM){
		modeManager->SetMode(MODE_BUS_PRIORITY);
	}
	currentState = ST_IDLE;
}

void CMDServer::loopReset(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "Daily reset encountered");

	for(uint8 i = 0; i < 30; i++){
		usleep(1000000);
	}

	logger->Log(LOGGER_LEVEL_ERROR, "Daily reset failed, performing reboot");
	system("reboot");
	currentState = ST_IDLE;
}

// --------- Beacon ----------------------------------------------------------------------------------------------------
// Note: this function is configured to be called in the subsystem loop defined in SubsystemServer.cpp
// since we can configure it to be called whenever we like, we can effectively set the beacon rate


void CMDServer::serializeBeacon(uint8 * buf, std::size_t size) {
	Serialize ser(buf, size);

	ser.serialize_int32(beacon.currentMode);
	ser.serialize_uint8(beacon.subPowerState);
	ser.serialize_int32(beacon.uptime);

	beacon.epsHS.update(buf, size, ser.get_serialize_index(), ser.get_deserialize_index());
	beacon.epsHS.serialize();

	beacon.acsHS.update(buf, size, beacon.epsHS.get_serialize_index(), beacon.epsHS.get_deserialize_index());
	beacon.acsHS.serialize();

	beacon.cdhHS.update(buf, size, beacon.acsHS.get_serialize_index(), beacon.acsHS.get_deserialize_index());
	beacon.cdhHS.serialize();

	beacon.pldHS.update(buf, size, beacon.cdhHS.get_serialize_index(), beacon.cdhHS.get_deserialize_index());
	beacon.pldHS.serialize();
}

bool CMDServer::CheckHealthStatus(void) {
	if(currentState == ST_IDLE || currentState == ST_PASS_PREP || currentState == ST_LOGIN || currentState == ST_POST_PASS){
		CreateBeacon();
		return true;
	}
	return false;
}

void CMDServer::CreateBeacon() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "Creating Beacon");

	beaconValid = false;

	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	beacon.currentMode = modeManager->GetMode();

	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	beacon.subPowerState = 		(cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS] & 0x01) |
										(cdhServer->subsystemPowerStates[HARDWARE_LOCATION_COM] & 0x02) |
										(cdhServer->subsystemPowerStates[HARDWARE_LOCATION_GPS] & 0x04) |
										(cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD] & 0x08);

	beacon.cdhHS = cdhServer->CDHState;

	beacon.uptime = getTimeInMillis() - startTime;

	EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	beacon.epsHS = epsServer->EPSState;

	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	beacon.acsHS = acsServer->ACSState;

	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	beacon.pldHS = pldServer->PLDState;

	beaconValid = true;
}

bool CMDServer::CheckForBeacon() {
	if(beaconValid) {
		beaconValid = false;
		return true;
	} else {
		return false;
	}
}

void CMDServer::bootConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CMD_CONFIG, "r");
	uint8 buffer[CMDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: NULL CMD config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CMDConfiguration.size, fp) == CMDConfiguration.size) {
		CMDConfiguration.update(buffer, CMDConfiguration.size, 0, 0);
		CMDConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: successfully booted CMD configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: error reading CMD config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool CMDServer::updateConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CMD_CFG_UP, "r");
	uint8 buffer[CMDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: NULL CMD config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CMDConfiguration.size, fp) == CMDConfiguration.size) {
		CMDConfiguration.update(buffer, CMDConfiguration.size, 0, 0);
		CMDConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "CMDServer: successfully updated CMD configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: error reading CMD config file, cannot update");
		fclose(fp);
		return false;
	}
}

} // End of namespace Servers
} // End of namespace AllStar
