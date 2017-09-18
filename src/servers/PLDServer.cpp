/*
 * PayloadServer.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Michael
 *
 *  Last Updated by: Robert Belter
 */

#include "servers/PLDServer.h"
#include "servers/PLDStdTasks.h"
#include "servers/FMGServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/ERRServer.h"
#include "servers/FileSystem.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include "servers/CDHServer.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

PLDConfig PLDServer::PLDConfiguration(RAD_CHUNK_SIZE);
uint16 PLDServer::PLDDataNumber = 1;

PLDServer::PLDServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn, 1000, 20), Singleton(), tftp_pid(-1) { }

PLDServer::~PLDServer() { }

PLDServer & PLDServer::operator=(const PLDServer & source) {
	if (this == &source) {
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool PLDServer::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized());
}

// ------------------------------------ State Machine ------------------------------------------------------------
void PLDServer::loopInit() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Info("PLDServer: initializing");
	TLM_PLD_SERVER_STARTED();

	PLDDataNumber = PLDReadDataNumber();

	bootConfig();

	currentState = ST_IDLE;
	// TODO: synchronize clock with GPS
}

void PLDServer::loopIdle() {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_BUS_PRIORITY:
		if (cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD]) {
			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
		}
		break;
	case MODE_PLD_PRIORITY:
		currentState = ST_STARTUP;
		break;
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}
}

void PLDServer::loopStartup() {
	ERRServer * errServer = static_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD]) {
		cdhServer->resetAssert(HARDWARE_LOCATION_PLD);
		cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
		usleep(300000);
		cdhServer->resetDeassert(HARDWARE_LOCATION_PLD);

		// delay while PLD boots
		wdmAsleep();
		usleep(4000000);
		wdmAlive();
	}

	if (PLDTestAlive()) {
		if (!PLDSelfCheck()) {
			errServer->SendError(ERR_PLD_SELFCHECK);
			wdmAsleep();
			usleep(3000000);
			wdmAlive();
			currentState = ST_IDLE;
			return;
		}

		logger->Info("PLD passed self check");
	} else {
		errServer->SendError(ERR_PLD_NOTALIVE);
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
		currentState = ST_IDLE;
		return;
	}

	wdmAlive();
	ACPPacket * turnOnScience = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_START_SCIENCE);
	ACPPacket * ret = DispatchPacket(turnOnScience);
	if (ret->getOpcode() != PLD_START_SCIENCE) {
		//TODO Error handling
		currentState = ST_IDLE;
	}
	delete ret;

	// fork a process to manage the data collection over tftp
	PLDDataNumber = PLDUpdateDataNumber();
	sprintf(dataFile, "RAD_%u", PLDDataNumber);
	int pid = fork();
	if (pid == 0) {
		char * argv[] = {"/usr/bin/tftp", "-g", "-r", dataFile, "10.14.134.207", NULL};
		if (execve("/usr/bin/tftp", argv, {NULL}) == -1) {
			// TODO: handle thiscd
		}
		exit(0);
	}


	//PLD is on and in science mode. Goto science loop
	tftp_pid = pid;
	sleepTime = 3000;
	currentState = ST_SCIENCE;
}

// Graceful shutdown
void PLDServer::loopShutdown() {
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// turn off science mode
	ACPPacket * turnOffScience = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, SUBSYSTEM_RESET_CMD);
	ACPPacket * ret = DispatchPacket(turnOffScience);
	delete ret;

	// wait, then turn off the payload
	wdmAsleep();
	usleep(3000000);
	wdmAlive();
	cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);

	if (tftp_pid != -1) {
		// kill the tftp process if it's still running
		kill(tftp_pid, SIGINT);
		tftp_pid = -1;

		//set up archivename and command strings
		char archiveName[100];
		char command[300];
		//create archive name string for path to dataFile
		sprintf(archiveName, RAD_FILE_PATH "/%s", dataFile);
		// get how many files it was split into by dividing the dataFile size by the number of bytes per chunk
		ifstream in(archiveName, ifstream::ate | ifstream::binary);
		int f_bytes = in.tellg();
		int n_splits = f_bytes/PLDConfiguration.chunkSize;
		in.close();
		// split the file within the location using the same name (tags on 000,001,002,ect.)
		sprintf(command,"split -b %d -d -a 3 %s %s", PLDConfiguration.chunkSize,archiveName,archiveName);
		wdmAsleep();
		// runs the split command in the system
		system(command);
		wdmAlive();

		// for loop through the number of splits created
		for(int i = 0; 1 <= n_splits; i++){
			int i2 = 0; // middle 0 if i < 10
			string num = ""; // string to assign the numbers too, to reference each split file
			if(i >= 10){
				num = to_string(i); // if i >=10 only need i to creat the "000" tag on the file
			}
			else{
				num = to_string(i2) + to_string(i); // other wise need i2 and i
			}
			// gets archive name we wish to create a .tar.gz compressed file for each chunk
			sprintf(archiveName, RAD_FILE_PATH "/%s0%s.tar.gz",dataFile,num.c_str());
			// sets up command to compress the file we have into the path we created in the archiveName
			sprintf(command,"tar -czf %s " RAD_FILE_PATH "/%s0%s", archiveName,dataFile,num.c_str());
			wdmAsleep();
			// runs the command on the system
			system(command);
			wdmAlive();
			// create a different archiveName referencing just the individual chunks
			sprintf(archiveName, RAD_FILE_PATH "/%s0%s",dataFile,num.c_str());
			// removes the chunks to save some space
			remove(archiveName);
		}
		// removes the dataFile to save space
		remove(dataFile);




		/*
		OLD WAY DONE BY COMPRESSING FIRST THEN CHUNKING

		sprintf(archiveName, RAD_FILE_PATH "/%s.tar.gz", dataFile);
		sprintf(command, "tar -czf %s %s", archiveName, dataFile);
		wdmAsleep();
		system(command);
		wdmAlive();
		remove(dataFile);


		// split the data
		sprintf(command, "split -b %d -d -a 3 %s %s.", PLDConfiguration.chunkSize, archiveName, archiveName);
		wdmAsleep();
		system(command);
		wdmAlive();
		remove(archiveName);
		*/
	}

	//PLD is off. Goto idle loop
	sleepTime = 1000;
	currentState = ST_IDLE;
}



// Used for file

void PLDServer::loopScience() {
	// Make sure PLD hasn't been turned off due to a fault, if it has, go back into startup
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	if (cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD] == false) {
		currentState = ST_STARTUP;
	}

	// Stay in science mode until we move out of PLD_PRIORITY
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if (modeManager->GetMode() != MODE_PLD_PRIORITY) {
		currentState = ST_SHUTDOWN;
	}

	// TODO: figure out when to use the SPI backup
//	ACPPacket * dataRequest = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_BACKUP_SEND_SCIENCE);
//	ACPPacket * data = DispatchPacket(dataRequest);

//	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//	logger->Info("PLDServer: received data %d", data->getLength());
}

void PLDServer::loopReset() {
	PLDPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_IDLE;
}

// -------------------------------------------- PLD Methods ---------------------------------------------
bool PLDServer::CheckHealthStatus() {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if (modeManager->GetMode() == MODE_PLD_PRIORITY) {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

		ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, HEALTH_STATUS_CMD);
		ACPPacket * HSRet = DispatchPacket(HSQuery);

		if (HSRet == NULL) {
			logger->Error("PLDServer: NULL HSRet");
			return false;
		}

		if (HSRet->getLength() != PLDStatus::size) {
			logger->Warning("PLDServer: CheckHealthStatus(): incorrect message length!");

			//TODO: return error?
			return false;
		} else {
			logger->Info("PLDServer: CheckHealthStatus(): packet dispatched, HSRet acquired");

			// Parse buffer
			uint8 * msgPtr = HSRet->getMessageBuff();
			if (msgPtr==NULL) {
				logger->Error("PLDServer: CheckHealthStatus(): NULL msgPtr");
				return false;
			}

			// deserialize the message
			PLDState.update(msgPtr, PLDState.size, 0, 0);
			PLDState.deserialize();

			logger->Debug("PLD H&S: Power fault:     %u", PLDState.powerFault);
			logger->Debug("PLD H&S: Motor speed:     %u", PLDState.motorSpeed);
			for(uint8 i = 0; i < 10; i++){
				logger->Debug("PLD H&S: Thermistor %d:     %u", i, PLDState.thermistors[i]);
			}
			logger->Debug("PLD H&S: ACS Working:     %u", PLDState.adcDataWorking);
			logger->Debug("PLD H&S: Control:         %u", PLDState.control);

			int32 currTime = getTimeInSec();
			if (currTime >= (lastHSTLog + 60)) {
				lastHSTLog = currTime;

				// add the current spacecraft time to the log buffer
				uint8 * buffer = new uint8[PLDStatus::size + sizeof(int32)];
				AddUInt32(buffer, currTime);

				// add the PLD H&S to the buffer
				PLDState.update(buffer, PLDStatus::size, 4, 0);
				PLDState.serialize();

				FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
				fmgServer->Log(DESTINATION_PLD_HST, buffer, PLDStatus::size + sizeof(int32));
			}

			return true;
		}
	}
	return true;
}

void PLDServer::bootConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(PLD_CONFIG, "r");
	uint8 buffer[PLDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("PLDServer: NULL PLD config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), PLDConfiguration.size, fp) == PLDConfiguration.size) {
		PLDConfiguration.update(buffer, PLDConfiguration.size, 0, 0);
		PLDConfiguration.deserialize();
		logger->Info("PLDServer: successfully booted PLD configs");
		fclose(fp);
		return;
	} else {
		logger->Error("PLDServer: error reading PLD config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool PLDServer::updateConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(PLD_CFG_UP, "r");
	uint8 buffer[PLDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("PLDServer: NULL PLD config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), PLDConfiguration.size, fp) == PLDConfiguration.size) {
		PLDConfiguration.update(buffer, PLDConfiguration.size, 0, 0);
		PLDConfiguration.deserialize();
		logger->Info("PLDServer: successfully updated PLD configs");
		fclose(fp);
		return true;
	} else {
		logger->Error("PLDServer: error reading PLD config file, cannot update");
		fclose(fp);
		return false;
	}
}

} // end namespace servers
} // end namespace AllStar
