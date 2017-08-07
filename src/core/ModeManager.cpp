/*
 * ModeManager.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

namespace AllStar {
namespace Core {

bool ModeManager::validTransition[MODE_NUM_MODES][MODE_NUM_MODES] = {{false}};

ModeManager::ModeManager(void)
: Singleton(), mode(MODE_BUS_PRIORITY) {
	//For the time being, only allow transitions to and from bus priority
	for (int i = MODE_FIRST_MODE; i< MODE_NUM_MODES; i++)
		for (int j = MODE_FIRST_MODE; j< MODE_NUM_MODES; j++)
			validTransition[i][j] = false;
	for (int i = MODE_FIRST_MODE; i< MODE_NUM_MODES; i++) {
		validTransition[MODE_BUS_PRIORITY][i] = true;
		validTransition[i][MODE_BUS_PRIORITY] = true;
	}
}

ModeManager::~ModeManager(void) { }

bool ModeManager::IsFullyInitialized(void){
	return true;
}

SystemModeEnum ModeManager::GetMode(void){
	return mode;
}

bool ModeManager::SetMode(SystemModeEnum newMode){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Debug("SetMode called");

	logger->Debug("----SetMode: Before take lock");
	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		logger->Debug("----SetMode: Current Mode is not NULL");

		if (newMode < MODE_FIRST_MODE && newMode >= MODE_NUM_MODES){
			logger->Error("----SetMode: Invalid mode");
			this->GiveLock();
			return false;
		}

		if (!validTransition[this->mode][newMode]){
			this->GiveLock();
			return false;
		}

		// log the switch (we might already be in the mode, but the information is useful regardless)
		TLM_MODE_SWITCH(newMode);

		if (newMode == this->mode){
			this->GiveLock();
			return false;
		}

		switch(newMode){
		case MODE_BUS_PRIORITY:
			logger->Info("Setting mode to " "\x1b[32m" "MODE_BUS_PRIORITY" "\x1b[0m");
			break;
		case MODE_PLD_PRIORITY:
			logger->Info("Setting mode to " "\x1b[32m" "MODE_PLD_PRIORITY" "\x1b[0m");
			break;
		case MODE_COM:
			logger->Info("Setting mode to " "\x1b[32m" "MODE_COM" "\x1b[0m");
			break;
		case MODE_RESET:
			logger->Info("Setting mode to " "\x1b[32m" "MODE_RESET" "\x1b[0m");
			break;
		default:
			logger->Warning("Unknown mode selected!");
			break;
		}

		this->mode = newMode;

		this->GiveLock();
		logger->Debug("------------SetMode: success");
		return true;
	}else{
		return false;
	}
}

}
}
