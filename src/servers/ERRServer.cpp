/*
* ERRServer.cpp
*
*  Created on: June 1, 2017
*      Author: Alex St. Clair
*/
#include "servers/ERRServer.h"
#include "core/StdTypes.h"
#include "util/Logger.h"

namespace AllStar {
namespace Servers {

// --------------------- Necessary Methods --------------------------
ERRServer::ERRServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton()
{ }

ERRServer::~ERRServer() { }

bool ERRServer::IsFullyInitialized(void) {
	return Singleton::IsFullyInitialized();
}


// ---------------------- Subsystem Loop ----------------------------
void ERRServer::SubsystemLoop() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ErrorOpcodeType error;
	int64 lastWakeTime;

	while (1) {
		wdmAlive();
		lastWakeTime = getTimeInSec();

		if (!ErrorQueue.empty()) {
			if (this->TakeLock(MAX_BLOCK_TIME) && !ErrorQueue.empty()) {
				error = ErrorQueue.front();
				ErrorQueue.pop();

				SortError(error);
				this->GiveLock();
			} else {
				logger->Log(LOGGER_LEVEL_WARN, "ERRServer: SubsystemLoop() error taking lock");
			}
		}

		waitUntil(lastWakeTime, 100);
	}
}


// ----------------------- Other Methods ----------------------------
void ERRServer::SendError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		ErrorQueue.push(error);
		this->GiveLock();
	} else {
		logger->Log(LOGGER_LEVEL_WARN, "ERRServer: SendError() unable to take lock");
	}
}


void ERRServer::SortError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (error >= ERR_ACS_MIN && error < ERR_ACS_MAX) {
		ACSError(error);
	} else if (error >= ERR_CDH_MIN && error < ERR_CDH_MAX) {
		CDHError(error);
	} else if (error >= ERR_CMD_MIN && error < ERR_CMD_MAX) {
		CMDError(error);
	} else if (error >= ERR_COM_MIN && error < ERR_COM_MAX) {
		COMError(error);
	} else if (error >= ERR_EPS_MIN && error < ERR_EPS_MAX) {
		EPSError(error);
	} else if (error >= ERR_FMG_MIN && error < ERR_FMG_MAX) {
		FMGError(error);
	} else if (error >= ERR_GPS_MIN && error < ERR_GPS_MAX) {
		GPSError(error);
	} else if (error >= ERR_PLD_MIN && error < ERR_PLD_MAX) {
		PLDError(error);
	} else if (error >= ERR_SCH_MIN && error < ERR_SCH_MAX) {
		SCHError(error);
	} else if (error >= ERR_MAX) {
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: error out of range!");
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown error %d", error);
	}
}


void ERRServer::ACSError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown ACS error");
	}
}


void ERRServer::CDHError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown CDH error");
	}
}


void ERRServer::CMDError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown CMD error");
	}
}


void ERRServer::COMError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown COM error");
	}
}


void ERRServer::EPSError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	case ERR_EPS_NOTALIVE:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: eps not alive");
		break;
	case ERR_EPS_SELFCHECK:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: eps failed self check");
		break;
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown EPS error");
	}
}


void ERRServer::FMGError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown FMG error");
	}
}


void ERRServer::GPSError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown GPS error");
	}
}


void ERRServer::PLDError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown PLD error");
	}
}


void ERRServer::SCHError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "ERRServer: unknown SCH error");
	}
}

}
}
