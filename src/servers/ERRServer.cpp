/*
* ERRServer.cpp
*
*  Created on: June 1, 2017
*      Author: Alex St. Clair
*/
#include "servers/ERRServer.h"
#include "servers/CDHServer.h"
#include "core/StdTypes.h"
#include "util/Logger.h"
#include "util/TLM.h"

namespace AllStar {
namespace Servers {

// --------------------- Necessary Methods --------------------------
ERRServer::ERRServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton() {
	ACSNotAliveCount = 0;
	COMNotAliveCount = 0;
	EPSNotAliveCount = 0;
	PLDNotAliveCount = 0;
}

ERRServer::~ERRServer() { }

bool ERRServer::IsFullyInitialized(void) {
	return Singleton::IsFullyInitialized();
}


// ---------------------- Subsystem Loop ----------------------------
void ERRServer::SubsystemLoop() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ErrorOpcodeType error;
	int64 lastWakeTime;

	TLM_ERR_SERVER_STARTED();

	while (1) {
		wdmAlive();
		lastWakeTime = getTimeInMillis();

		if (!ErrorQueue.empty()) {
			if (this->TakeLock(MAX_BLOCK_TIME)) {
				error = ErrorQueue.front();
				ErrorQueue.pop();
				this->GiveLock();

				SortError(error);
			} else {
				logger->Warning("ERRServer: SubsystemLoop() error taking lock");
			}
		}

		waitUntil(lastWakeTime, 100);
	}
}


// ----------------------- Other Methods ----------------------------
void ERRServer::SendError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		ErrorQueue.push(error);
		this->GiveLock();
	} else {
		logger->Warning("ERRServer: SendError() unable to take lock");
	}
}


void ERRServer::SortError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

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
	} else if (error >= ERR_GEN_MIN && error < ERR_GEN_MAX) {
		GENError(error);
	} else if (error >= ERR_MAX) {
		logger->Error("ERRServer: error out of range!");
	} else {
		logger->Error("ERRServer: unknown error %d", error);
	}
}


void ERRServer::ACSError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {

	// ACS does not respond to aliveness check
	case ERR_ACS_NOTALIVE:
		logger->Error("ERRServer: ACS is not alive");
		if (++ACSNotAliveCount >= 10) {
			CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			cdhServer->subPowerOff(HARDWARE_LOCATION_ACS);
			logger->Fatal("ERRServer: powering ACS off!");
			ACSNotAliveCount = 0;
		}
		break;

	// ACS does not pass self check
	case ERR_ACS_SELFCHECK:
		logger->Error("ERRServer: ACS failed self check");
		break;

	// This should never happen
	default:
		logger->Error("ERRServer: unknown ACS error");
	}
}


void ERRServer::CDHError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Error("ERRServer: unknown CDH error");
	}
}


void ERRServer::CMDError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Error("ERRServer: unknown CMD error");
	}
}


void ERRServer::COMError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {

	// COM does not respond to an aliveness check
	case ERR_COM_NOTALIVE:
		logger->Error("ERRServer: COM is not alive");
		if (++COMNotAliveCount >= 10) {
			CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			cdhServer->subPowerOff(HARDWARE_LOCATION_COM);
			logger->Fatal("ERRServer: powering COM off!");
			COMNotAliveCount = 0;
		}
		break;

	// COM does not pass its self check
	case ERR_COM_SELFCHECK:
		logger->Error("ERRServer: com failed self check");
		break;

	// This should never happen
	default:
		logger->Error("ERRServer: unknown COM error");
	}
}


void ERRServer::EPSError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	case ERR_EPS_NOTALIVE:
		logger->Error("ERRServer: eps not alive");
		// FIXME: need to add EPS reset
		break;
	case ERR_EPS_SELFCHECK:
		logger->Error("ERRServer: eps failed self check");
		break;
	default:
		logger->Error("ERRServer: unknown EPS error");
	}
}


void ERRServer::FMGError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Error("ERRServer: unknown FMG error");
	}
}


void ERRServer::GPSError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Error("ERRServer: unknown GPS error");
	}
}


void ERRServer::PLDError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {

	// PLD does not respond to an aliveness check
	case ERR_PLD_NOTALIVE:
		logger->Error("ERRServer: PLD is not alive");
		if (++PLDNotAliveCount >= 10) {
			CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
			logger->Fatal("ERRServer: powering PLD off!");
			PLDNotAliveCount = 0;
		}
		break;

	// PLD does not pass its self check
	case ERR_PLD_SELFCHECK:
		logger->Error("ERRServer: PLD failed self check");
		break;

	// This should never happen
	default:
		logger->Error("ERRServer: unknown PLD error");
	}
}


void ERRServer::SCHError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Error("ERRServer: unknown SCH error");
	}
}


void ERRServer::GENError(ErrorOpcodeType error) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	switch (error) {
	default:
		logger->Error("ERRServer: unknown GEN error");
	}
}

}
}
