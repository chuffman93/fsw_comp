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
#include "servers/GPSServer.h"
#include "servers/FileSystem.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include "HAL/SPI_Server.h"
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

CMDConfig CMDServer::CMDConfiguration(360,1000,1000,10,10,10,900);

CMDServer::CMDServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), numFilesDWN(0), currFileNum(0), uftp_pid(-1), downlinkInProgress(false) {
	startTime = getTimeInSec();
	comTime = 0;
	CMDConfiguration.resetPeriod = 6000; // 3 hrs FIXME: change for flight
	CMDConfiguration.fileChunkSize = 102400; // 100 kB
	CMDConfiguration.maxDownlinkSize = 15728640; // 15 MB
	CMDConfiguration.beaconPeriod = 15;
	CMDConfiguration.increasedBeaconPeriod = 8;
	CMDConfiguration.comTimeout = 900;
	beaconValid = false;
	com = false;
}

CMDServer::~CMDServer(){
}

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
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("CMDServer: Initializing");
	TLM_CMD_SERVER_STARTED();

	// setup for uftp
	uftpSetup();

	bootConfig();
	hsDelays = CMDConfiguration.beaconPeriod;

	currentState = ST_IDLE;
}

void CMDServer::loopIdle(void) {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Dispatcher * dispatcher = static_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

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
	if (checkForSOT() != 0) {
		logger->Warning("CMDServer: surprise COM! Preparing to enter COM Mode");
		SCHServer * schServer = static_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
		schServer->RequestCOMMode();
		wdmAsleep();
		usleep(5000000);
		wdmAlive();
		currentState = ST_PASS_PREP;
		return;
	}

	// Monitor daily reset
	int64 currTime = getTimeInSec();
	if (currTime > startTime + 400000) {
		// an arbitrarily large amount of time has passed, assume the time was updated from GPS and reset the start time
		startTime = currTime;
	} else if (currTime > (startTime + CMDConfiguration.resetPeriod) && (modeManager->GetMode() == MODE_BUS_PRIORITY)) { // time to reboot
		SCHServer * schServer = static_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
		schServer->RequestReset();
		currentState = ST_RESET;
		return;
	}

	dispatcher->CleanRXQueue();
}

void CMDServer::loopPassPrep(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	hsDelays = CMDConfiguration.increasedBeaconPeriod; // up the beacon rate

	// see if the fmgServer has prepped Verbose H&S
	if(fmgServer->isComReady()){
		logger->Info("CMDServer: finished COM pass prep");
		currentState = ST_LOGIN;
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Error("CMDServer: FMGServer never prepped for COM, COM pass over");
		hsDelays = CMDConfiguration.beaconPeriod; // reset beacon period
		currentState = ST_IDLE;
	}else if (com == false){
		comTime = getTimeInSec();
		com = true;
	}
}

void CMDServer::loopLogin(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// block until the start of transmission file has been uplinked
	int SOTresult = checkForSOT();
	if(SOTresult == 1){
		logger->Info("CMDServer: ground login successful, skipping VH&S");
		hsDelays = CMDConfiguration.beaconPeriod; // set the beacon to normal rate
		currentState = ST_UPLINK;
	} else if(SOTresult == 2){
		logger->Info("CMDServer: ground login successful, sending VH&S");
		hsDelays = CMDConfiguration.beaconPeriod; // set the beacon to normal rate
		currentState = ST_VERBOSE_HS;
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Info("CMDServer: no login, COM pass over");
		hsDelays = CMDConfiguration.beaconPeriod; // set the beacon to normal rate
		currentState = ST_POST_PASS;
	}else if (com == false){
		comTime = getTimeInSec();
		com = true;
	}else if (comTime+CMDConfiguration.comTimeout<getTimeInSec()){
		currentState = ST_POST_PASS;
		logger->Info("CMDServer: com timed out, COM pass over");
	}
}

void CMDServer::loopVerboseHS(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// downlink the Verbose H&S file
	logger->Info("CMDServer: Verbose H&S unimplemented");
	currentState = ST_UPLINK;
	if (comTime+CMDConfiguration.comTimeout<getTimeInSec()){
		currentState = ST_POST_PASS;
		logger->Info("CMDServer: com timed out, COM pass over");
		}
}

void CMDServer::loopUplink(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	// check if the EOT file has been uplinked
	if(access(EOT_PATH,F_OK) == 0) {
		// if EOT, check for IEF
		if(access(IEF_PATH,F_OK) == 0) {
			logger->Info("CMDServer: uplink concluded, beginning immediate execution");
			IEFline = NULL;
			IEFlen = 0;
			if (openIEF(&IEFfp, &IEFline, &IEFlen)) {
				currentState = ST_IMMEDIATE_EXECUTION;
			} else {
				// TODO: send IEF NAK
			}
		} else {
			logger->Info("CMDServer: uplink concluded, starting planned downlink");
			currentState = ST_DOWNLINK_PREP;
		}
	}

	// make sure that the COM pass hasn't concluded
	if(modeManager->GetMode() != MODE_COM){
		logger->Error("CMDServer: uplink not finished, COM pass over");
		currentState = ST_POST_PASS;
	}else if (comTime+CMDConfiguration.comTimeout<getTimeInSec()){
		currentState = ST_POST_PASS;
		logger->Info("CMDServer: com timed out, COM pass over");
	}
}

void CMDServer::loopImmediateExecution() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_COM){
		logger->Error("CMDServer: immediate execution timeout, COM pass over");
		if (downlinkInProgress) {
			downlinkInProgress = false;
			kill(uftp_pid, SIGINT);
			uftp_pid = -1;
		}
		currentState = ST_POST_PASS;
	}else if (downlinkInProgress) {
		int status;
		if (waitpid(uftp_pid, &status, WNOHANG) != 0) {
			TLM_UFTP_RETURN_STATUS(status);
			downlinkInProgress = false;
			uftp_pid = -1;
		}
		return;
	}else if (comTime+CMDConfiguration.comTimeout<getTimeInSec()){
		currentState = ST_POST_PASS;
		logger->Info("CMDServer: com timed out, COM pass over");
	}

	uint8 lineNum = 1;
	while (parseIEFLine(IEFfp, &IEFline, &IEFlen, lineNum++)) {
		wdmAlive();
		if (downlinkInProgress) {
			return;
		}
	}

	logger->Info("Finished immediate execution, resuming uplink");

	fclose(IEFfp);

	// Clear immed directory
	system("rm -rf " IMMED_DIRECTORY "/*");

	// Clear uplink directory
	system("rm -rf " UPLINK_DIRECTORY "/*");

	currentState = ST_UPLINK;
}

void CMDServer::loopDownlinkPrep(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	numFilesDWN = getNumFiles((char *) DOWNLINK_DIRECTORY);
	currFileNum = 1;

	logger->Info("CMDServer: %d files to downlink", numFilesDWN);
	currentState = ST_DOWNLINK;
}

void CMDServer::loopDownlink() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	string filename;

	parseDFL();

	if (modeManager->GetMode() != MODE_COM) {
		logger->Error("CMDServer: downlink timed out, COM pass over");
		numFilesDWN = 0;
		currFileNum = 1;
		kill(uftp_pid, SIGINT);
		downlinkInProgress = false;
		uftp_pid = -1;
		currentState = ST_POST_PASS;
	}

	if (downlinkInProgress) {
		int status;
		if (waitpid(uftp_pid, &status, WNOHANG) != 0) {
			TLM_UFTP_RETURN_STATUS(status);
			downlinkInProgress = false;
			uftp_pid = -1;
		} else {
			return;
		}
	}

	if (currFileNum < numFilesDWN + 1 && !downlinkInProgress) {
		filename = getDownlinkFile(currFileNum++);
		DownlinkFile(filename);
	} else {
		logger->Info("CMDServer: downlink finished");
		numFilesDWN = 0;
		currFileNum = 1;
		currentState = ST_POST_PASS;
	}

	if (comTime+CMDConfiguration.comTimeout<getTimeInSec()){
		currentState = ST_POST_PASS;
		logger->Info("CMDServer: com timed out, COM pass over");
		}
}

void CMDServer::loopPostPass() {
	SCHServer * schServer = static_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	parsePPE();

	// Clear downlink directory
	system("rm -rf " DOWNLINK_DIRECTORY "/*");

	parseDLT();
	parseDRF();
	processUplinkFiles();

	// Clear uplink directory
	system("rm -rf " UPLINK_DIRECTORY "/*");

	if (access(SCHEDULE_FILE, F_OK) != -1) {
		schServer->UpdateNewSchedule();
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
	} else if (modeManager->GetMode() == MODE_COM) {
		schServer->EndCOMMode();
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
	}

	com = false;
	comTime = 0;
	currentState = ST_IDLE;
}

void CMDServer::loopReset(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("Daily reset encountered");
	TLM_DAILY_RESET();
	char cmd[100];
	sprintf(cmd, "echo %u > " REBOOT_TIME_FILE, getTimeInSec() + CMDConfiguration.expectedRebootDuration);
	system(cmd);

	wdmAsleep();
	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}
	wdmAlive();

	logger->Error("Daily reset failed, performing reboot");
	sprintf(cmd, "echo %u > " REBOOT_TIME_FILE, getTimeInSec() + 5);
	system(cmd);
	system("reboot");
	currentState = ST_IDLE;
}

// --------- Beacon ----------------------------------------------------------------------------------------------------
// Note: this function is configured to be called in the subsystem loop defined in SubsystemServer.cpp
// since we can configure it to be called whenever we like, we can effectively set the beacon rate

bool CMDServer::CheckHealthStatus(void) {
	if(currentState == ST_IDLE || currentState == ST_PASS_PREP || currentState == ST_LOGIN || currentState == ST_POST_PASS){
		UpdateBeacon();
		return true;
	}
	return false;
}

void CMDServer::UpdateBeacon() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("Creating Beacon");

	BeaconStruct tempBeacon;

	tempBeacon.satTime = getTimeInSec();

	GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	GPSPositionTime * gpsData = gpsServer->GetGPSDataPtr();
	tempBeacon.GPSWeek = gpsData->GPSWeek;
	tempBeacon.GPSSec = gpsData->GPSSec;
	tempBeacon.xPosition = gpsData->posX;
	tempBeacon.yPosition = gpsData->posY;
	tempBeacon.zPosition = gpsData->posZ;
	tempBeacon.xVelocity = gpsData->velX;
	tempBeacon.yVelocity = gpsData->velY;
	tempBeacon.zVelocity = gpsData->velZ;

	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	tempBeacon.systemMode = modeManager->GetMode();

	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	tempBeacon.subpowerStates = ((uint8) cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]) << 3 |
							((uint8) cdhServer->subsystemPowerStates[HARDWARE_LOCATION_COM]) << 2 |
							((uint8) cdhServer->subsystemPowerStates[HARDWARE_LOCATION_GPS]) << 1 |
							((uint8) cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD]);
	tempBeacon.memory = cdhServer->CDHState.memory;
	tempBeacon.cpu15 = cdhServer->CDHState.cpu15;

	FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	tempBeacon.epochNumber = fmgServer->bootCount;

	PLDServer * pldServer = static_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	tempBeacon.radNumber = pldServer->PLDDataNumber;

	SPI_HALServer * spiServer = static_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	tempBeacon.spiSent = spiServer->packetsSentTX;
	tempBeacon.spiDropped = spiServer->packetsDroppedTX;

	EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	tempBeacon.batteryCap = epsServer->EPSState.remainingCapacity;

	ACSServer * acsServer = static_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	tempBeacon.acsMode = acsServer->ACSState.mode;

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		beacon = tempBeacon;
		this->GiveLock();
		beaconValid = true;
	} else {
		logger->Warning("CMDServer: unable to take lock to update beacon");
	}

	// log beacons to a file for testing purposes
	uint8 buffer[BeaconStruct::size];
	beacon.update(buffer,BeaconStruct::size);
	beacon.serialize();
	FILE * fp = fopen(HOME_DIRECTORY "/BeaconLog", "a");
	fwrite(buffer, sizeof(uint8), BeaconStruct::size, fp);
	fclose(fp);
}

bool CMDServer::CheckForBeacon() {
	return beaconValid;
}

void CMDServer::serializeBeacon(uint8 * buffer) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		beacon.update(buffer, BeaconStruct::size);
		beacon.serialize();
		this->GiveLock();
		beaconValid = false;
	} else {
		logger->Warning("CMDServer: unable to take lock to serialize beacon");
	}
}

void CMDServer::bootConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CMD_CONFIG, "r");
	uint8 buffer[CMDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("CMDServer: NULL CMD config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CMDConfiguration.size, fp) == CMDConfiguration.size) {
		CMDConfiguration.update(buffer, CMDConfiguration.size, 0, 0);
		CMDConfiguration.deserialize();
		logger->Info("CMDServer: successfully booted CMD configs");
		fclose(fp);
		return;
	} else {
		logger->Error("CMDServer: error reading CMD config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool CMDServer::updateConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CMD_CFG_UP, "r");
	uint8 buffer[CMDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("CMDServer: NULL CMD config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CMDConfiguration.size, fp) == CMDConfiguration.size) {
		CMDConfiguration.update(buffer, CMDConfiguration.size, 0, 0);
		CMDConfiguration.deserialize();
		logger->Info("CMDServer: successfully updated CMD configs");
		fclose(fp);
		return true;
	} else {
		logger->Error("CMDServer: error reading CMD config file, cannot update");
		fclose(fp);
		return false;
	}
}

// Note: the filename must be the absolute path
void CMDServer::DownlinkFile(string fileName) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(access(fileName.c_str(), F_OK) != 0) {
		logger->Error("downlinkFile: file doesn't exist!");
		return;
	}

	int pid = fork();
	if (pid == 0) {
		char * argv[] = {UFTP_PATH, "-Y", "aes256-gcm", "-h", "sha256", "-I", "sl0", "-H", "1.1.1.2", "-s", "50", "-x", "1", "-m", "10", (char *) fileName.c_str(), NULL};
		logger->Info("CMDServer: downlinking file");
		execve(UFTP_PATH, argv, {NULL});
		exit(0);
	}

	downlinkInProgress = true;
	uftp_pid = pid;
}

} // End of namespace Servers
} // End of namespace AllStar
