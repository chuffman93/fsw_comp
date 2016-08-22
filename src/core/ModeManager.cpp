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

#define MODE_DEBUG			1

#if MODE_DEBUG
#include <iostream>
#define DEBUG_MODE_COUT(m) std::cout << m << std::endl;
#else
#define DEBUG_MODE_COUT(m)
#endif // DEBUG

using namespace std;

namespace AllStar{
namespace Core{

bool ModeManager::validTransition[MODE_NUM_MODES][MODE_NUM_MODES] = {false};

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

bool ModeManager::Attach(ModeListener & listener){
	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		pair<set<ModeListener *>::const_iterator,bool> result = listeners.insert(&listener);
		this->GiveLock();
		return result.second;
	}else{
		return false;
	}
}

bool ModeManager::Detach(ModeListener & listener){
	bool ret;

	if (true == this->TakeLock(MAX_BLOCK_TIME)){
		ret = (listeners.erase(&listener) > 0);
		this->GiveLock();
		return ret;
	}else{
		return false;
	}
}

SystemModeEnum ModeManager::GetMode(void){
	return mode;
}

bool ModeManager::SetMode(SystemModeEnum newMode, LocationIDType server){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("SetMode called", LOGGER_LEVEL_DEBUG);
	switch(newMode){
	case MODE_ACCESS:
		logger->Log("Setting mode to MODE_ACCESS", LOGGER_LEVEL_INFO);
		break;
	case MODE_STARTUP:
		logger->Log("Setting mode to MODE_STARTUP", LOGGER_LEVEL_INFO);
		break;
	case MODE_BUS_PRIORITY:
		logger->Log("Setting mode to MODE_BUS_PRIORITY", LOGGER_LEVEL_INFO);
		break;
	case MODE_PLD_PRIORITY:
		logger->Log("Setting mode to MODE_PLD_PRIORITY", LOGGER_LEVEL_INFO);
		break;
	case MODE_ERROR:
		logger->Log("Setting mode to MODE_ERROR", LOGGER_LEVEL_INFO);
		break;
	case MODE_COM:
		logger->Log("Setting mode to MODE_COM", LOGGER_LEVEL_INFO);
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

		NotifyAll();

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

void ModeManager::NotifyAll(void){
	for (set<ModeListener *>::iterator i = listeners.begin(); i != listeners.end(); ++i){
		(*i)->Update(this->mode);
	}
}

}
}
