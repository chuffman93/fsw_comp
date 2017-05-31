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
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;
using namespace AllStar::Core;

namespace AllStar {
namespace Servers {

FMGConfig FMGServer::FMGConfiguration(0);
uint32 FMGServer::bootCount = 0;

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
	}
	file_open = false;

	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	file = fopen(file_name.c_str(), "a");
	if (file == NULL)
		logger->Log(LOGGER_LEVEL_WARN, "Error opening file");
	file_open = true;
	bytes_written = 0;
}

void FileManager::GetFileName() {
	int time = getTimeInSec();
	char fileName[100];
	sprintf(fileName, "%s/CUR/%s_%d_%d", file_path.c_str(), TLM_type.c_str(), FMGServer::bootCount, time);
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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (buf_size >= MAX_FILE_SIZE) {
		logger->Log(LOGGER_LEVEL_WARN, "Telemetry larger than file size");
	} else {
		// Check if file is open
		if (!file_open) {
			logger->Log(LOGGER_LEVEL_DEBUG, "File not open");
			OpenNewFile();
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
					ACPLogger(ACP_FILE_PATH, "ACP"),
					CMDLogger(CMD_FILE_PATH, "CMD"),
					DGNLogger(DGN_FILE_PATH, "DGN"),
					ERRLogger(ERR_FILE_PATH, "ERR"),
					FSSLogger(FSS_FILE_PATH, "FSS"),
					GENLogger(GEN_FILE_PATH, "GEN"),
					HSTLogger(HST_FILE_PATH, "HST"),
					MODLogger(MOD_FILE_PATH, "MOD"),
					SSSLogger(SSS_FILE_PATH, "SSS"),
					SWPLogger(SWP_FILE_PATH, "SWP"),
					RADLogger(RAD_FILE_PATH, "RAD"),
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
	DGNLogger.CloseFile();
	ERRLogger.CloseFile();
	FSSLogger.CloseFile();
	GENLogger.CloseFile();
	HSTLogger.CloseFile();
	MODLogger.CloseFile();
	SSSLogger.CloseFile();
	SWPLogger.CloseFile();
    RADLogger.CloseFile();
}

void FMGServer::Log(FILServerDestinationEnum dest, uint8 * buf, size_t size) {
	FilePacket packet;
	packet.buffer = buf;
	packet.size = size;
	packet.dest = dest;
	FileQueue.push(packet);
}

void FMGServer::CallLog() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * buf = FileQueue.front().buffer;
    size_t size = FileQueue.front().size;

	switch (FileQueue.front().dest) {
	case DESTINATION_ACP:
		if ( !ACPLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to ACP File");
		}
		break;
	case DESTINATION_CMD:
		if ( !CMDLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Command File");
		}
		break;
	case DESTINATION_DGN:
		if ( !DGNLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Diagnostic File");
		}
		break;
	case DESTINATION_ERR:
		if ( !ERRLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Error File");
		}
		break;
	case DESTINATION_FSS:
		if ( !FSSLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to File System File");
		}
		break;
	case DESTINATION_GEN:
		if ( !GENLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to General File");
		}
		break;
	case DESTINATION_HST:
		if( !HSTLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Health and Status File");
		}
		break;
	case DESTINATION_MOD:
		if( !MODLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Mode File");
		}
		break;
	case DESTINATION_SSS:
		if ( !SSSLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Science System File");
		}
		break;
	case DESTINATION_SWP:
		if ( !SWPLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to HotSwap File");
		}
		break;
	case DESTINATION_RAD:
		if ( !RADLogger.Log(buf, size) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Rad File");
		}
		break;
	default:
		logger->Log(LOGGER_LEVEL_WARN, "FMGServer: Unknown destination!");
		break;
	}

	free(buf);
	FileQueue.pop();
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
	//nothing to initialize
	bootConfig();
	bootCount = loadBootCount();
	currentState = ST_RUN;
}

void FMGServer::loopRun(void) {
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

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
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));


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
uint32 FMGServer::loadBootCount() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (access(EPOCH_FILE, F_OK) != -1) {
		FILE * fp = fopen(EPOCH_FILE, "r");

		if (fp == NULL) {
			logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: bad fp for epoch file");
			return 0;
		}

		char * line = NULL;
		size_t len = 0;
		ssize_t bytesRead = getline(&line, &len, fp);

		if (bytesRead < 1 || bytesRead > 5) {
			logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: problem reading epoch file");
			return 0;
		}

		return strtoul(line, NULL, 10);
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: unable to open epoch file");
	}
	return 0;
}

void FMGServer::bootConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(FMG_CONFIG, "r");
	uint8 buffer[FMGConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: NULL FMG config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), FMGConfiguration.size, fp) == FMGConfiguration.size) {
		FMGConfiguration.update(buffer, FMGConfiguration.size, 0, 0);
		FMGConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "FMGServer: successfully booted FMG configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: error reading FMG config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool FMGServer::updateConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(FMG_CFG_UP, "r");
	uint8 buffer[FMGConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: NULL FMG config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), FMGConfiguration.size, fp) == FMGConfiguration.size) {
		FMGConfiguration.update(buffer, FMGConfiguration.size, 0, 0);
		FMGConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "FMGServer: successfully updated FMG configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "FMGServer: error reading FMG config file, cannot update");
		fclose(fp);
		return false;
	}
}


} // End of namespace Servers
} // End of namespace AllStar
