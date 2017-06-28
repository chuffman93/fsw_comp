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
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"
#include "servers/ERRServer.h"
#include "servers/FileSystem.h"
#include "servers/FMGServer.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "util/Logger.h"
#include "util/TLM.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACSStatus ACSServer::ACSState(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
ACSConfig ACSServer::ACSConfiguration(0);

// -------------------------------------- Necessary Methods --------------------------------------
ACSServer::ACSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn, ACS_SLEEP_TIME, ACS_HS_DELAYS), Singleton(), testsRun(false) { }

ACSServer::~ACSServer() { }

ACSServer & ACSServer::operator=(const ACSServer & source) {
	if (this == &source) {
		return *this;
	}

	SubsystemServer::operator =(source);
	return *this;
}

bool ACSServer::IsFullyInitialized(void) {
	return Singleton::IsFullyInitialized();
}

// -------------------------------------------- State Machine --------------------------------------------
void ACSServer::loopInit() {
	ERRServer * errServer = static_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	TLM_ACS_SERVER_STARTED();

	ACSState.mode = ACS_MODE_UNKNOWN;

	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]) {
		cdhServer->resetAssert(HARDWARE_LOCATION_ACS);
		cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
		usleep(100000);
		cdhServer->resetDeassert(HARDWARE_LOCATION_ACS);

		// delay while ACS boots up
		wdmAsleep();
		usleep(4000000);
		wdmAlive();
	}

	if (ACSTestAlive()) {
		if (!ACSSelfCheck()) {
			errServer->SendError(ERR_ACS_SELFCHECK);
			wdmAsleep();
			usleep(3000000);
			wdmAlive();
			return;
		}

		logger->Log(LOGGER_LEVEL_INFO, "ACS passed self check");

		bootConfig();

		currentState = ST_DETUMBLE;
	} else {
		errServer->SendError(ERR_ACS_NOTALIVE);
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
	}
}

void ACSServer::loopDetumble() {
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS])
		currentState = ST_INIT;

	if (ACSState.mode != ACS_MODE_DETUMBLE) {
		if (ACSDetumble())
			ACSState.mode = ACS_MODE_DETUMBLE;
	} else if (isDetumbled()) {
		currentState = ST_SUN_SOAK;
	}
}

void ACSServer::loopSunSoak() {
	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS])
		currentState = ST_INIT;

	// check for state transitions from mode switches
	SystemModeEnum currentMode = modeManager->GetMode();
	switch (currentMode) {
	case MODE_COM:
		currentState = ST_COM_START;
		break;
	case MODE_PLD_PRIORITY:
		currentState = ST_PLD_START;
		break;
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}

	if (ACSState.mode != ACS_MODE_SUN) {
		if (ACSPointSun())
			ACSState.mode = ACS_MODE_SUN;
	}

	ACSSendGPS();
}

void ACSServer::loopPLDStart() {
	if (ACSPointNadir()) {
		ACSState.mode = ACS_MODE_NADIR;
		currentState = ST_PLD_POINTING;
		sleepTime = 1000;
		hsDelays = 15;
	} else {
		currentState = ST_SUN_SOAK;
	}
}

void ACSServer::loopPLDPointing() {
	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]) {
		currentState = ST_INIT;
	}

	if (modeManager->GetMode() != MODE_PLD_PRIORITY)
		currentState = ST_PLD_STOP;

	if (ACSState.mode != ACS_MODE_NADIR) {
		if (ACSPointNadir())
			ACSState.mode = ACS_MODE_NADIR;
	}

	ACSSendGPS();
}

void ACSServer::loopPLDStop() {
	sleepTime = 5000;
	hsDelays = 3;
	currentState = ST_SUN_SOAK;
}

void ACSServer::loopCOMStart() {
	if (ACSPointGND()) {
		ACSState.mode = ACS_MODE_GND;
		currentState = ST_COM_POINTING;
	} else {
		currentState = ST_SUN_SOAK;
	}
}

void ACSServer::loopCOMPointing() {
	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]) {
		currentState = ST_INIT;
	}

	if (modeManager->GetMode() != MODE_COM)
		currentState = ST_COM_STOP;

	if (ACSState.mode != ACS_POINT_GND) {
		if (ACSPointGND())
			ACSState.mode = ACS_POINT_GND;
	}

	ACSSendGPS();
}

void ACSServer::loopCOMStop() {
	currentState = ST_SUN_SOAK;
}

void ACSServer::loopReset() {
	ACSPrepReset();

	for (uint8 i = 0; i < 60; i++) {
		usleep(1000000);
	}

	currentState = ST_SUN_SOAK;
}

// -------------------------------------------- ACS Methods --------------------------------------------
bool ACSServer::CheckHealthStatus() {
	if (currentState == ST_INIT) {
		return false;
	}

	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if (HSRet == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL HSRet");
		return false;
	}

	if (HSRet->getLength() != ACSState.size) {
		logger->Log(LOGGER_LEVEL_WARN, "ACSServer: CheckHealthStatus(): incorrect message length! %u", HSRet->getLength());
		return false;
	} else {
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");

		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if (msgPtr==NULL) {
			logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: CheckHealthStatus(): NULL msgPtr");
			return false;
		}

		// deserialize the buffer
		ACSState.update(msgPtr,ACSState.size,0,0);
		ACSState.deserialize();

		int32 currTime = getTimeInSec();
		if (currTime >= (lastHSTLog + 60)) {
			lastHSTLog = currTime;

			// add the current spacecraft time to the log buffer
			uint8 * buffer = new uint8[ACSStatus::size + sizeof(int32)];
			AddUInt32(buffer, currTime);

			// add the ACS H&S to the buffer
			ACSState.update(buffer, ACSStatus::size, 4, 0);
			ACSState.serialize();

			FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
			fmgServer->Log(DESTINATION_ACS_HST, buffer, ACSStatus::size + sizeof(int32));
		}

		return true;
	}
}

void ACSServer::bootConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(ACS_CONFIG, "r");
	uint8 buffer[ACSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL ACS config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), ACSConfiguration.size, fp) == ACSConfiguration.size) {
		ACSConfiguration.update(buffer, ACSConfiguration.size, 0, 0);
		ACSConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: successfully booted ACS configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: error reading ACS config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool ACSServer::updateConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(ACS_CFG_UP, "r");
	uint8 buffer[ACSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL ACS config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), ACSConfiguration.size, fp) == ACSConfiguration.size) {
		ACSConfiguration.update(buffer, ACSConfiguration.size, 0, 0);
		ACSConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: successfully updated ACS configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: error reading ACS config file, cannot update");
		fclose(fp);
		return false;
	}
}

bool ACSServer::isDetumbled() {
	// TODO: determine if detumbled
	return true;
}

} // End Namespace servers
} // End Namespace AllStar
