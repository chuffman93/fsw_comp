/*
 * ModeManager.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
#include "TestMode.h"
#endif

using namespace std;

namespace AllStar{
namespace Core{

bool ModeManager::validTransition[MODE_NUM_MODES][MODE_NUM_MODES] = {{false}};

ModeManager::ModeManager(void)
: Singleton(), mode(MODE_STARTUP){
}

ModeManager::~ModeManager(void){
}

void ModeManager::Initialize(void){
	//For the time being, only allow transitions to and from bus priority
	for(int i = MODE_FIRST_MODE; i< MODE_NUM_MODES; i++)
		for(int j = MODE_FIRST_MODE; j< MODE_NUM_MODES; j++)
			validTransition[i][j] = false;
	for(int i = MODE_FIRST_MODE; i< MODE_NUM_MODES; i++){
		validTransition[MODE_BUS_PRIORITY][i] = true;
		validTransition[i][MODE_BUS_PRIORITY] = true;
	}
}

bool ModeManager::IsFullyInitialized(void){
	return true;
}

#ifdef TEST
void ModeManager::Destroy(void){
	//SystemMode::Destroy();
}
#endif

SystemModeEnum ModeManager::GetMode(void){
	return mode;
}

bool ModeManager::SetMode(SystemModeEnum newMode){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("SetMode called", LOGGER_LEVEL_DEBUG);
	switch(newMode){
	case MODE_ACCESS:
		logger->Log("Setting mode to " "\x1b[32m" "MODE_ACCESS" "\x1b[0m", LOGGER_LEVEL_INFO);
		break;
	case MODE_STARTUP:
		logger->Log("Setting mode to " "\x1b[32m" "MODE_STARTUP" "\x1b[0m", LOGGER_LEVEL_INFO);
		break;
	case MODE_BUS_PRIORITY:
		logger->Log("Setting mode to " "\x1b[32m" "MODE_BUS_PRIORITY" "\x1b[0m", LOGGER_LEVEL_INFO);
		break;
	case MODE_PLD_PRIORITY:
		logger->Log("Setting mode to " "\x1b[32m" "MODE_PLD_PRIORITY" "\x1b[0m", LOGGER_LEVEL_INFO);
		break;
	case MODE_COM:
		logger->Log("Setting mode to " "\x1b[32m" "MODE_COM" "\x1b[0m", LOGGER_LEVEL_INFO);
		break;
	case MODE_DIAGNOSTIC:
		logger->Log("Setting mode to " "\x1b[32m" "MODE_DIAGNOSTIC" "\x1b[0m", LOGGER_LEVEL_INFO);
		break;
	default:
		logger->Log("Unknown mode selected!", LOGGER_LEVEL_WARN);
		break;
	}

	logger->Log("----SetMode: Before take lock", LOGGER_LEVEL_DEBUG);
	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		logger->Log("----SetMode: Current Mode is not NULL", LOGGER_LEVEL_DEBUG);

		if (newMode < MODE_FIRST_MODE && newMode >= MODE_NUM_MODES){
			logger->Log("----SetMode: Invalid mode", LOGGER_LEVEL_ERROR);
			this->GiveLock();
			return false;
		}

		if (newMode == this->mode){
			this->GiveLock();
			return false;
		}

		if (!validTransition[this->mode][newMode]){
			this->GiveLock();
			return false;
		}

		// TODO: need mode logger here
		this->mode = newMode;
		logger->Log("Switching mode!", LOGGER_LEVEL_INFO);

		this->GiveLock();
		logger->Log("------------SetMode: success", LOGGER_LEVEL_DEBUG);
		return true;
	}else{
		return false;
	}
}

}
}
