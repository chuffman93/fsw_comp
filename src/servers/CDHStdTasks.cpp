/*
 * CDHStdTasks.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FMGServer.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "util/itoa.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <sys/sysinfo.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <sys/stat.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar {
namespace Servers {

struct stat sb;

//------------------------------------------- Message Handlers -------------------------------------------
void CDHSystemInfo(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	struct sysinfo si;
	if (sysinfo(&si) != 0) {
		logger->Error("CDHStdTasks: CDHCPUUsage(): Error");
		return;
	}

	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->CDHState.cpu1  = si.loads[0]/6553.6;
	cdhServer->CDHState.cpu5  = si.loads[1]/6553.6;
	cdhServer->CDHState.cpu15 = si.loads[2]/6553.6;
	cdhServer->CDHState.memory = 100.0*(259964928.0 - ((float) si.freeram)) / (259964928.0); //hard-coded total ram: 100*(total - free)/total = percent use

	logger->Debug("CDHCPUUsage(): Checking loads");
	logger->Debug("    CPU  1 min: %f", cdhServer->CDHState.cpu1);
	logger->Debug("    CPU  5 min: %f", cdhServer->CDHState.cpu5);
	logger->Debug("    CPU 15 min: %f", cdhServer->CDHState.cpu15);
	logger->Debug("    CPU memory: %f", cdhServer->CDHState.memory);
}

void CDHTempStart(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Start all of the sensors
	bool sensorResult = false;
	bool someSuccess = false;
	bool allSuccess = true;
	for (uint8 bus = 0; bus < 4; bus++) {
		for (uint8 sensor = 0; sensor < 16; sensor++) {
			sensorResult = StartTempSensor(bus+1,sensor);
			someSuccess |= sensorResult;
			allSuccess &= sensorResult;
		}
	}

	if (allSuccess) {
		//logger->Debug("CDHStdTasks: started all temp sensors");
	} else if (someSuccess) {
		//logger->Debug("CDHStdTasks: error starting some temp sensors");
	} else {
		logger->Error("CDHStdTasks: error starting all temp sensors");
	}
}

void CDHTempRead(float * tempArray) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Debug("CDHStdTasks: CDHTempRead(): Reading temp sensors");

	// Read and add to list
	for (uint8 bus = 0; bus < 4; bus++) {
		for (uint8 sensor = 0; sensor < 16; sensor++) {
			tempArray[(bus * 16) + sensor] = ReadTempSensor(bus+1,sensor);
		}
	}
}

// Helper Functions ---------------------------------------------------------------
bool StartTempSensor(int bus, int sensor) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char busChar[3];
	itoa(bus, busChar, 10);
	char sensChar[3];
	itoa(sensor, sensChar, 10);

	// make the filename and check if it exists
	string tempFile = "/sys/bus/w1/devices/w1_bus_master";
	tempFile.append(busChar);
	tempFile.append("/TEMP");
	tempFile.append(sensChar);
	tempFile.append("/temp");
	if (access(tempFile.c_str(), F_OK) == -1) {
		return false;
	}

	// create the start command
	string start = "echo 1 > /sys/bus/w1/devices/w1_bus_master";
	start.append(busChar);
	start.append("/TEMP");
	start.append(sensChar);
	start.append("/start");

	// start sensor
	if (system(start.c_str()) == -1) {
		//logger->Debug("CDHStdTasks: StartTempSensor(): Error Starting Sensor!");
		return false;
	}
	return true;
}

float ReadTempSensor(int bus, int sensor) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	// create filename
	char * temp = new char[1];
	string read = "/sys/bus/w1/devices/w1_bus_master";
	itoa(bus, temp, 10);
	read.append(temp);
	read.append("/TEMP");
	itoa(sensor, temp, 10);
	read.append(temp);
	read.append("/temp");
	delete temp;

	FILE * fp;
	fp = fopen(read.c_str(), "r");

	bool isGood = false;
	if (fp) {
		char * c = new char[1];
		char * tempRead = new char[9];
		int tempHold;
		int count = 0;
		float temperature;

		// Get temperature part of string
		while ((*c = fgetc(fp)) != '\n') {
			tempRead[count] = *c;
			count++;
		}

		// Get float value
		sscanf(tempRead, "t=%d", &tempHold);
		temperature = (float) tempHold / 1000.0;

		// Check validity
		for (int i = 0; i < 28; i++) {
			*c = fgetc(fp);
			if (i==27) {
				if (*c=='V') {
					isGood = true;
				}
			}
		}

		// Act on validity
		if (isGood) {
			//logger->Debug("CDHStdTasks: ReadTempSensor: Good data (%0.2f) from sensor %d on bus %d", temperature, sensor, bus);
			delete c;
			delete tempRead;
			fclose(fp);
			return temperature;
		} else {
			//logger->Debug("CDHStdTasks: ReadTempSensor: Bad data from sensor %d on bus %d!", sensor, bus);
			delete c;
			delete tempRead;
			fclose(fp);
			return -300;
		}
	} else {
		//logger->Debug("CDHStdTasks: ReadTempSensor: Error opening file: sensor %d on bus %d", sensor, bus);
		return -301;
	}
}

void prepPowerGPIOs(void) {
	// COM
	if (!(stat("/sys/class/gpio/pioB15/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 47 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB15/direction\"");
	}

	// ACS
	if (!(stat("/sys/class/gpio/pioB25/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 57 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB25/direction\"");
	}

	// PLD
	if (!(stat("/sys/class/gpio/pioB17/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 49 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB17/direction\"");
	}

	// GPS
	if (!(stat("/sys/class/gpio/pioB27/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 59 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB27/direction\"");
	}

	// AUXCOM
	if (!(stat("/sys/class/gpio/pioB11/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 43 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB11/direction\"");
	}

	//-----------------------RESET LINES--------------------------------------
	//EPS
	if (!(stat("/sys/class/gpio/pioE10/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 138 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioE10/direction\"");
	}

	//COM
	if (!(stat("/sys/class/gpio/pioA11/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 11 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioA11/direction\"");
	}
	system("echo 1 > \"/sys/class/gpio/pioA11/value\"");

	//ACS
	if (!(stat("/sys/class/gpio/pioA12/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 12 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioA12/direction\"");
	}

	//PLD
	if (!(stat("/sys/class/gpio/pioE11/", &sb) == 0 && S_ISDIR(sb.st_mode))) {
		system("echo 139 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioE11/direction\"");
	}
}

void toggleSubPower(HardwareLocationIDType subsystem, bool state) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	string cmd = "echo ";

	switch(subsystem){
	case HARDWARE_LOCATION_COM:
		if (state) {
			TLM_PWR_COM_ON();
			logger->Info("CDHStdTasks: Turning COM on");
			cmd.append("1 > \"/sys/class/gpio/pioB15/value\"");
		} else {
			TLM_PWR_COM_OFF();
			logger->Info("CDHStdTasks: Turning COM off");
			cmd.append("0 > \"/sys/class/gpio/pioB15/value\"");
		}
		break;
	case HARDWARE_LOCATION_ACS:
		if (state) {
			TLM_PWR_ACS_ON();
			logger->Info("CDHStdTasks: Turning ACS on");
			cmd.append("1 > \"/sys/class/gpio/pioB25/value\"");
		} else {
			TLM_PWR_ACS_OFF();
			logger->Info("CDHStdTasks: Turning ACS off");
			cmd.append("0 > \"/sys/class/gpio/pioB25/value\"");
		}
		break;
	case HARDWARE_LOCATION_PLD:
		if (state) {
			TLM_PWR_PLD_ON();
			logger->Info("CDHStdTasks: Turning PLD on");
			cmd.append("1 > \"/sys/class/gpio/pioB17/value\" ");
		} else {
			TLM_PWR_PLD_OFF();
			logger->Info("CDHStdTasks: Turning PLD off");
			cmd.append("0 > \"/sys/class/gpio/pioB17/value\" ");
		}
		break;
	case HARDWARE_LOCATION_GPS:
		if (state) {
			TLM_PWR_GPS_ON();
			logger->Info("CDHStdTasks: Turning GPS on");
			cmd.append("1 > \"/sys/class/gpio/pioB27/value\"");
		} else {
			TLM_PWR_GPS_OFF();
			logger->Info("CDHStdTasks: Turning GPS off");
			cmd.append("0 > \"/sys/class/gpio/pioB27/value\"");
		}
		break;
	default:
		TLM_PWR_INVALID();
		logger->Warning("CDHStdTasks: Invalid subsystem for power toggle");
		break;
	}

	system(cmd.c_str());
}

void toggleResetLine(HardwareLocationIDType subsystem, bool state) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	string cmd = "echo ";

	if (state) {
		cmd.append("1 > \"/sys/class/gpio/pio");
	} else {
		cmd.append("0 > \"/sys/class/gpio/pio");
	}

	switch (subsystem) {
	case HARDWARE_LOCATION_EPS:
		if (state) {
			TLM_RST_EPS_HI();
			logger->Info("CDHStdTasks: EPS reset line high");
			cmd.append("1 > \"/sys/class/gpio/pioE10/value\"");
		} else {
			TLM_RST_EPS_LO();
			logger->Info("CDHStdTasks: EPS reset line low");
			cmd.append("0 > \"/sys/class/gpio/pioE10/value\"");
		}
		break;
	case HARDWARE_LOCATION_COM:
		if (state) {
			TLM_RST_COM_HI();
			logger->Info("CDHStdTasks: COM reset line high");
			cmd.append("1 > \"/sys/class/gpio/pioA11/value\"");
		} else {
			TLM_RST_COM_LO();
			logger->Info("CDHStdTasks: COM reset line low");
			cmd.append("0 > \"/sys/class/gpio/pioA11/value\"");
		}
		break;
	case HARDWARE_LOCATION_ACS:
		if (state) {
			TLM_RST_ACS_HI();
			logger->Info("CDHStdTasks: ACS reset line high");
			cmd.append("1 > \"/sys/class/gpio/pioA12/value\"");
		} else {
			TLM_RST_ACS_LO();
			logger->Info("CDHStdTasks: ACS reset line low");
			cmd.append("0 > \"/sys/class/gpio/pioA12/value\"");
		}
		break;
	case HARDWARE_LOCATION_PLD:
		if (state) {
			TLM_RST_PLD_HI();
			logger->Info("CDHStdTasks: PLD reset line high");
			cmd.append("1 > \"/sys/class/gpio/pioE11/value\"");
		} else {
			TLM_RST_PLD_LO();
			logger->Info("CDHStdTasks: PLD reset line low");
			cmd.append("0 > \"/sys/class/gpio/pioE11/value\"");
		}
		break;
	default:
		TLM_RST_INVALID();
		logger->Warning("CDHStdTasks: Invalid subsystem for reset");
		return;
	}

	system(cmd.c_str());
}

}
}
