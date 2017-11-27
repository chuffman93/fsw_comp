/*
* SCHServer.cpp
*
*  Created on: July 7, 2016
*      Author: Robert Belter
*/
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/CMDServer.h"
#include "servers/FileSystem.h"
#include "servers/Structs.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"

#include <sys/mman.h>
#include <stdio.h>
#include <list>
#include <iterator>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

SCHServer::SCHServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton(), surpriseCOM(false), newSchedule(true), endCOM(false), resetRequest(false), lastWakeTime(0), modeEnterTime(0), lastBusEnter(0), itemEntered(false)
{ }

SCHServer::~SCHServer() { }

SCHServer & SCHServer::operator=(const SCHServer & source) {
	if (this == &source) {
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool SCHServer::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized());
}

void SCHServer::RequestCOMMode(void) {
	surpriseCOM = true;
}

void SCHServer::EndCOMMode(void) {
	endCOM = true;
}

void SCHServer::UpdateNewSchedule(void) {
	newSchedule = true;
}

void SCHServer::RequestReset(void) {
	resetRequest = true;
}

bool SCHServer::LoadDefaultScheduleConfigurations(char * filename) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(filename, "rb");
	uint8 buffer[SCHItem::size * SCHEDULE_MAX_SIZE];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("SCHServer: NULL default schedule file pointer, cannot boot");
		SetDefaultBusSchedule();
		return false;
	}

	// check file size (and reset fp to beginning)
	fseek(fp,0,SEEK_END);
	int file_size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	if (file_size == 0 || (file_size % SCHItem::size) != 0) {
		logger->Error("SCHServer: Invalid default schedule file size");
		SetDefaultBusSchedule();
		fclose(fp);
		return false;
	}

	// read and update the configs
	size_t bytes_read = fread(buffer, sizeof(uint8), file_size, fp);
	if (bytes_read == file_size) {
		defaultSchedule.clear();

		// deserialize and check each schedule item
		for (uint8 i = 0; i < bytes_read / SCHItem::size; i++) {
			DefaultArray[i].update(buffer, bytes_read, SCHItem::size*i, SCHItem::size*i);
			DefaultArray[i].deserialize();
			DefaultArray[i].timeout += getTimeInSec();
			if ((DefaultArray[i].enter_mode < 0 || DefaultArray[i].enter_mode > MODE_NUM_MODES) ||
					(DefaultArray[i].mode < 0 || DefaultArray[i].mode > MODE_NUM_MODES)) {
				// error in file
				logger->Error("SCHServer: default schedule item out of range");
				SetDefaultBusSchedule();
				fclose(fp);
				return false;
			}
		}

		// push the new items to the default schedule
		UpdateDefaultSchedule(bytes_read / SCHItem::size);

		logger->Info("SCHServer: successfully loaded default schedule");
		fclose(fp);
		return true;
	} else {
		logger->Error("SCHServer: error reading default schedule");
		SetDefaultBusSchedule();
		fclose(fp);
		return false;
	}
}

void SCHServer::UpdateDefaultSchedule(int numItems) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	for (uint8 j = 0; j < 5; j++) {
		if (this->TakeLock(MAX_BLOCK_TIME)) {
			for (uint8 i = 0; i < numItems; i++) {
				defaultSchedule.push_back(DefaultArray[i]);
			}
			this->GiveLock();
			return;
		}
	}

	logger->Error("SCHServer: unable to update default schedule");
	// TODO: handle this
}

void SCHServer::SetDefaultBusSchedule(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	double targetPosition[2] = {0, 0};
	SCHItem defaultItem(targetPosition, -1, 1, 1, MODE_BUS_PRIORITY, 300);
	for (uint8 i = 0; i < 5; i++) {
		if (this->TakeLock(MAX_BLOCK_TIME)) {
			defaultSchedule.clear();
			defaultSchedule.push_back(defaultItem);
			this->GiveLock();
			return;
		}
	}

	logger->Error("SCHServer: unable to set default bus due to lock!");
	// TODO: handle this
	}

void SCHServer::SubsystemLoop(void) {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	GPSServer * gpsServer = static_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	TLM_SCH_SERVER_STARTED();

	modeManager->SetMode(MODE_BUS_PRIORITY);

	currentSchedule = std::list<SCHItem>();
	itemEntered = false;
	lastBusEnter = getTimeInSec();

	// remove any old schedule that may be left behind by a reboot
	if (access(SCHEDULE_FILE, F_OK) != -1) {
		remove(SCHEDULE_FILE);
	}

	while(1) {
		wdmAlive();
		lastWakeTime = getTimeInMillis();

		if (newSchedule) {
			newSchedule = false;
			modeManager->SetMode(MODE_BUS_PRIORITY);
			lastBusEnter = getTimeInSec();
			itemEntered = false;
			while (!currentSchedule.empty()) {
				currentSchedule.pop_front();
			}
		}

		if (currentSchedule.empty()) {
			logger->Info("Fetching next schedule");
			LoadNextSchedule();
		}

		if (surpriseCOM) {
			surpriseCOM = false;
			modeManager->SetMode(MODE_BUS_PRIORITY);
			if(itemEntered){
				itemEntered = false;
				currentSchedule.pop_front();
			}
			double targetPosition[2] = {0, 0};
			SCHItem newCOM(targetPosition, -1, 1, 1, MODE_COM, DEFAULT_COM_DURATION);
			currentSchedule.push_front(newCOM);
		}

		if (endCOM) {
			endCOM = false;
			if (modeManager->GetMode() == MODE_COM) {
				logger->Info("SCHServer: COM pass over");
				modeManager->SetMode(MODE_BUS_PRIORITY);
				lastBusEnter = getTimeInSec();
				currentSchedule.pop_front();
				itemEntered = false;
			}
		}

		if (resetRequest) {
			resetRequest = false;
			modeManager->SetMode(MODE_RESET);
			double targetPosition[2] = {0, 0};
			SCHItem resetItem(targetPosition, -1, 1, getTimeInSec() + 15, MODE_RESET, 30);
			TLM_RESET_COMMANDED();
			itemEntered = false;
			currentSchedule.push_front(resetItem);
		}

		SCHItem currentEvent = currentSchedule.front();

		if (itemEntered) {
			if (currentEvent.duration < getTimeInSec() - modeEnterTime) {
				logger->Info("Schedule event duration exceeded, entering Bus Priority Mode");
				modeManager->SetMode(MODE_BUS_PRIORITY);
				lastBusEnter = getTimeInSec();
				currentSchedule.pop_front();
				itemEntered = false;
			}
		} else {
			bool inRange = gpsServer->DistanceTo(currentEvent.ecefPos) < currentEvent.radius;
			if (inRange) {
				logger->Info("In range of schedule event, setting mode");
				SystemModeEnum currentMode = modeManager->GetMode();

				if (currentMode == MODE_BUS_PRIORITY && ((lastBusEnter + 10) < getTimeInSec())) { // make sure we've been in bus priority mode for >10 seconds
					modeManager->SetMode((SystemModeEnum) currentEvent.mode);
					itemEntered = true;
					modeEnterTime = getTimeInSec();
				} else if (currentMode != MODE_BUS_PRIORITY) {
					modeManager->SetMode(MODE_BUS_PRIORITY);
					lastBusEnter = getTimeInSec();
				}
			} else if (currentEvent.timeout < getTimeInSec()) {
				if (currentEvent.enter_mode) {
					logger->Info("Schedule event timeout exceeded, entering mode");
					SystemModeEnum currentMode = modeManager->GetMode();

					if (currentMode == MODE_BUS_PRIORITY && ((lastBusEnter + 10) < getTimeInSec())) { // make sure we've been in bus priority mode for >10 seconds
						modeManager->SetMode((SystemModeEnum) currentEvent.mode);
						itemEntered = true;
						modeEnterTime = getTimeInSec();
					} else if (currentMode != MODE_BUS_PRIORITY) {
						modeManager->SetMode(MODE_BUS_PRIORITY);
						lastBusEnter = getTimeInSec();
					}
				} else {
					logger->Info("Schedule event timeout exceeded, skipping mode");
					modeManager->SetMode(MODE_BUS_PRIORITY);
					lastBusEnter = getTimeInSec();
					currentSchedule.pop_front();
					itemEntered = false;
				}
			}
		}

		waitUntil(lastWakeTime, 1000);
	}
}

void SCHServer::LoadDefaultSchedule(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	LoadDefaultScheduleConfigurations((char *) SCH_CONFIG);
	currentSchedule = defaultSchedule;
	logger->Info("Default Schedule Loaded");
	TLM_LOAD_DEFAULT_SCHEDULE();
	if (access(SCHEDULE_FILE, F_OK) != -1) {
		remove(SCHEDULE_FILE);
	}
}

void SCHServer::UpdateSchedule(int numItems) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	for (uint8 j = 0; j < 5; j++) {
		if (this->TakeLock(MAX_BLOCK_TIME)) {
			for (uint8 i = 0; i < numItems; i++) {
				currentSchedule.push_back(ScheduleArray[i]);
			}
			this->GiveLock();
			return;
		}
	}

	logger->Error("SCHServer: unable to update schedule");
	// TODO: handle this
}

int SCHServer::LoadNextSchedule(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	currentSchedule.clear();
	if (access(SCHEDULE_FILE, F_OK) == -1) {
		logger->Warning("LoadNextSchedule: No new schedule, loading default...");
		LoadDefaultSchedule();
		return -1;
	}

	FILE * fp = fopen(SCHEDULE_FILE, "rb");
	uint8 buffer[SCHItem::size * SCHEDULE_MAX_SIZE];

	cout << fp << endl;

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("SCHServer: unable to open new schedule, loading default...");
		TLM_SCHEDULE_BAD_OPEN();
		LoadDefaultSchedule();
		return false;
	}

	// check file size (and reset fp to beginning)
	fseek(fp,0,SEEK_END);
	int file_size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	if (file_size == 0 || (file_size % SCHItem::size) != 0) {
		TLM_SCHEDULE_BAD_SIZE();
		logger->Error("SCHServer: Invalid schedule file size, loading default...");
		cout << "The file size and size are: " << file_size << "  " << SCHItem::size << endl << endl;
		LoadDefaultSchedule();
		fclose(fp);
		return false;
	}

	// read and update the configs
	size_t bytes_read = fread(buffer, sizeof(uint8), file_size, fp);
	if (bytes_read == file_size) {
		// deserialize and check each schedule item
		for (uint8 i = 0; i < bytes_read / SCHItem::size; i++) {
			ScheduleArray[i].update(buffer, bytes_read, SCHItem::size*i, SCHItem::size*i);
			ScheduleArray[i].deserialize();
			ScheduleArray[i].timeout += getTimeInSec(); // FIXME: update to absolute timing
			if ((ScheduleArray[i].enter_mode < 0 || ScheduleArray[i].enter_mode > 1) ||
					(ScheduleArray[i].mode < 0 || ScheduleArray[i].mode > MODE_NUM_MODES)) {
				// error in file
				TLM_SCHEDULE_BAD_ITEM();
				logger->Error("SCHServer: schedule item out of range, loading default...");
				LoadDefaultSchedule();
				fclose(fp);
				return false;
			}
		}

		// push the new items to the default schedule
		UpdateSchedule(bytes_read / SCHItem::size);

		TLM_LOAD_NEW_SCHEDULE();
		logger->Info("SCHServer: successfully loaded new schedule");
		if (access(SCHEDULE_FILE, F_OK) != -1) {
			remove(SCHEDULE_FILE);
		}
		fclose(fp);
		return true;
	} else {
		logger->Error("SCHServer: error reading new schedule");
		TLM_SCHEDULE_BAD_READ();
		LoadDefaultSchedule();
		fclose(fp);
		return false;
	}
}

} // End of namespace Servers
} // End of namespace AllStar
