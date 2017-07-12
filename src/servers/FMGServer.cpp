/*
* FMGServer.cpp
*
*  Last Modified: Jack Dinkel 2/15/17
*/

#include "servers/FMGServer.h"
#include "servers/FileSystem.h"
#include "servers/Structs.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;
using namespace AllStar::Core;

namespace AllStar {
namespace Servers {

FMGConfig FMGServer::FMGConfiguration(0);
uint16 FMGServer::bootCount = 0;

// ----- File Manager functions ------------------------------------------------------------------------------------
FileManager::FileManager(string path, string tlmType) {
	file_path = path;
	TLM_type = tlmType;
	bytes_written = 0;
	file_open = false;
	file_name = "";
	file = NULL;
}

void FileManager::CloseFile() {
	// Close the file
	if (file_open) {
		fclose(file);
		file_open = false;
	} else {
		return;
	}

	FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	if (fmgServer->checkMoveFromCur() && strcmp("",file_name.c_str()) != 0) { // ensure that the filename exists
		MoveFile();
	}
}

void FileManager::MoveFile() {
	// Move the file out of CUR
	string new_file_name = file_name;
	new_file_name.erase(file_path.length(), 4);
	rename(file_name.c_str(), new_file_name.c_str());
	file_name = new_file_name;
}

void FileManager::OpenFile() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	file = fopen(file_name.c_str(), "a");
	if (file == NULL) {
		logger->Warning("Error opening file");
		file_open = false;
		return;
	}
	file_open = true;
	bytes_written = 0;
}

void FileManager::GetFileName() {
	int time = getTimeInSec();
	char fileName[100];
	sprintf(fileName, "%s/CUR/%s_%u_%d", file_path.c_str(), TLM_type.c_str(), FMGServer::bootCount, time);
	file_name = fileName;
}

void FileManager::OpenNewFile() {
	GetFileName();
	OpenFile();
}

void FileManager::Write(uint8 * buf, std::size_t buf_size) {
	fwrite(buf, sizeof(char), buf_size, file);
	fflush(file);
	bytes_written += buf_size + 1;
}

bool FileManager::Log(uint8 * buf, std::size_t buf_size){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (buf_size >= MAX_FILE_SIZE) {
		logger->Warning("Telemetry larger than file size");
	} else {
		// Check if file is open
		if (!file_open) {
			logger->Debug("File not open");
			OpenNewFile();
			if (!file_open) {
				printf("file not open");
				return false;
			}
		}

		if (bytes_written + buf_size + 1 < MAX_FILE_SIZE) {
			Write(buf, buf_size);
		} else { // File is full, open new file and write to it
			CloseFile();
			OpenNewFile();
			Write(buf, buf_size);
		}
	}
	return true;
}

void LogFSS(void){
	// Do SD cards work?
	// How full is each SD card?
}

// ----- FILServer functions ------------------------------------------------------------------------------------
FMGServer::FMGServer(string nameIn, LocationIDType idIn) :
					SubsystemServer(nameIn, idIn, 10, 1000),
					Singleton(),
					CMDLogger(CMD_FILE_PATH, "CMD"),
					ERRLogger(ERR_FILE_PATH, "ERR"),
					GENLogger(GEN_FILE_PATH, "GEN"),
					GPSLogger(GPS_FILE_PATH, "GPS"),
					MODLogger(MOD_FILE_PATH, "MOD"),
					PWRLogger(PWR_FILE_PATH, "PWR"),
					SPILogger(SPI_FILE_PATH, "SPI"),
					SWPLogger(SWP_FILE_PATH, "SWP"),
					THMLogger(THM_FILE_PATH, "THM"),
					ACS_HSTLogger(ACS_HST_PATH, "ACS"),
					CDH_HSTLogger(CDH_HST_PATH, "CDH"),
					COM_HSTLogger(COM_HST_PATH, "COM"),
					EPS_HSTLogger(EPS_HST_PATH, "EPS"),
					PLD_HSTLogger(PLD_HST_PATH, "PLD"),
					resetReady(false),
					comReady(false),
					move_from_CUR(false)
					{
}

FMGServer::~FMGServer() { }

bool FMGServer::IsFullyInitialized(void) {
	return Singleton::IsFullyInitialized();
}

FMGServer & FMGServer::operator=(const FMGServer & source) {
	if (this == &source) {
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void FMGServer::CloseAndMoveAllFiles() {
	CMDLogger.CloseFile();
	ERRLogger.CloseFile();
	GENLogger.CloseFile();
	GPSLogger.CloseFile();
	MODLogger.CloseFile();
	PWRLogger.CloseFile();
	SPILogger.CloseFile();
	SWPLogger.CloseFile();
	THMLogger.CloseFile();
	ACS_HSTLogger.CloseFile();
	CDH_HSTLogger.CloseFile();
	COM_HSTLogger.CloseFile();
	EPS_HSTLogger.CloseFile();
	PLD_HSTLogger.CloseFile();
}

void FMGServer::Log(FILServerDestinationEnum dest, uint8 * buf, size_t size) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FilePacket packet;
	packet.buffer = buf;
	packet.size = size;
	packet.dest = dest;

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		FileQueue.push(packet);
		this->GiveLock();
	} else {
		logger->Warning("FMGServer: Log() unable to take lock");
	}
}

void FMGServer::CallLog() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FilePacket packet;

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		packet = FileQueue.front();
		FileQueue.pop();
		this->GiveLock();
	} else {
		logger->Warning("FMGServer: CallLog() unable to take lock");
		return;
	}

	uint8 * buf = packet.buffer;
    size_t size = packet.size;

	switch (packet.dest) {
	case DESTINATION_CMD:
		if (!CMDLogger.Log(buf, size)) {
			logger->Warning("Error writing to Command File");
		}
		break;
	case DESTINATION_ERR:
		if (!ERRLogger.Log(buf, size)) {
			logger->Warning("Error writing to Error File");
		}
		break;
	case DESTINATION_GEN:
		if (!GENLogger.Log(buf, size)) {
			logger->Warning("Error writing to General File");
		}
		break;
	case DESTINATION_GPS:
		if (!GPSLogger.Log(buf, size)) {
			logger->Warning("Error writing to GPS File");
		}
		break;
	case DESTINATION_MOD:
		if (!MODLogger.Log(buf, size)) {
			logger->Warning("Error writing to Mode File");
		}
		break;
	case DESTINATION_PWR:
		if (!PWRLogger.Log(buf, size)) {
			logger->Warning("Error writing to Power File");
		}
		break;
	case DESTINATION_SPI:
		if (!SPILogger.Log(buf, size)) {
			logger->Warning("Error writing to SPI File");
		}
		break;
	case DESTINATION_SWP:
		if (!SWPLogger.Log(buf, size)) {
			logger->Warning("Error writing to HotSwap File");
		}
		break;
	case DESTINATION_THM:
		if (!THMLogger.Log(buf, size)) {
			logger->Warning("Error writing to Thermal File");
		}
		break;
	case DESTINATION_ACS_HST:
		if (!ACS_HSTLogger.Log(buf, size)) {
			logger->Warning("Error writing to ACS HST File");
		}
		break;
	case DESTINATION_CDH_HST:
		if (!CDH_HSTLogger.Log(buf, size)) {
			logger->Warning("Error writing to CDH HST File");
		}
		break;
	case DESTINATION_COM_HST:
		if ( !COM_HSTLogger.Log(buf, size) ) {
			logger->Warning("Error writing to COM HST File");
		}
		break;
	case DESTINATION_EPS_HST:
		if ( !EPS_HSTLogger.Log(buf, size) ) {
			logger->Warning("Error writing to EPS HST File");
		}
		break;
	case DESTINATION_PLD_HST:
		if ( !PLD_HSTLogger.Log(buf, size) ) {
			logger->Warning("Error writing to PLD HST File");
		}
		break;
	default:
		logger->Warning("FMGServer: Unknown destination!");
		break;
	}

	free(buf);
}

void FMGServer::PrepVerboseHST(void){
	// TODO: figure out what VHS is needed
	// tar the two most recent health and status files into verbose HST directory
//	char cmd[256];
//	sprintf(cmd, "tar -czf %s -C %s `ls -r %s | head -2`", VERBOSE_HST_DIRECTORY, HST_FILE_PATH, HST_FILE_PATH);
//	system(cmd);
}
// -----------------------------------------------------------------------------------------------------------------


// --------- State Machine -----------------------------------------------------------------------------------------
void FMGServer::loopInit(void) {
	TLM_FMG_SERVER_STARTED();
	system(CLEAR_CUR_DIRECTORIES_SCRIPT);
	bootConfig();
	bootCount = loadBootCount();
	currentState = ST_RUN;
}

void FMGServer::loopRun(void) {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	move_from_CUR = true;

	if (!FileQueue.empty()) {
		CallLog();
	} else if (modeManager->GetMode() == MODE_RESET) {
		currentState = ST_RESET;
	} else if (modeManager->GetMode() == MODE_COM) {
		currentState = ST_COM_PREP;
	}
}

void FMGServer::loopComPrep(void) {
	CloseAndMoveAllFiles();
	move_from_CUR = false;

	PrepVerboseHST();

	comReady = true;
	currentState = ST_COM;
}

void FMGServer::loopCom(void) {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if (!FileQueue.empty()){
		CallLog();
	} else if (modeManager->GetMode() != MODE_COM) {
		if (modeManager->GetMode() == MODE_RESET) {
			comReady = false;
			currentState = ST_RESET;
		} else {
			move_from_CUR = true;
			CloseAndMoveAllFiles();
			system(CLEAR_CUR_DIRECTORIES_SCRIPT);
			comReady = false;
			currentState = ST_RUN;
		}
	}
}

void FMGServer::loopReset(void) {
	move_from_CUR = true;
	CloseAndMoveAllFiles();
	resetReady = true;
	for (uint8 i = 0; i < 60; i++) {
		usleep(1000000);
	}

	resetReady = false;
	currentState = ST_RUN;
}

// -----------------------------------------------------------------------------------------------------------------
uint16 FMGServer::loadBootCount() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (access(EPOCH_FILE, F_OK) != -1) {
		FILE * fp = fopen(EPOCH_FILE, "r");

		if (fp == NULL) {
			logger->Error("FMGServer: bad fp for epoch file");
			return 0;
		}

		char * line = NULL;
		size_t len = 0;
		ssize_t bytesRead = getline(&line, &len, fp);

		if (bytesRead < 1 || bytesRead > 5) {
			logger->Error("FMGServer: problem reading epoch file");
			return 0;
		}

		return (uint16) strtoul(line, NULL, 10);
	} else {
		logger->Error("FMGServer: unable to open epoch file");
	}
	return 0;
}

void FMGServer::bootConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(FMG_CONFIG, "r");
	uint8 buffer[FMGConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("FMGServer: NULL FMG config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), FMGConfiguration.size, fp) == FMGConfiguration.size) {
		FMGConfiguration.update(buffer, FMGConfiguration.size, 0, 0);
		FMGConfiguration.deserialize();
		logger->Info("FMGServer: successfully booted FMG configs");
		fclose(fp);
		return;
	} else {
		logger->Error("FMGServer: error reading FMG config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool FMGServer::updateConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(FMG_CFG_UP, "r");
	uint8 buffer[FMGConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("FMGServer: NULL FMG config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), FMGConfiguration.size, fp) == FMGConfiguration.size) {
		FMGConfiguration.update(buffer, FMGConfiguration.size, 0, 0);
		FMGConfiguration.deserialize();
		logger->Info("FMGServer: successfully updated FMG configs");
		fclose(fp);
		return true;
	} else {
		logger->Error("FMGServer: error reading FMG config file, cannot update");
		fclose(fp);
		return false;
	}
}


} // End of namespace Servers
} // End of namespace AllStar
