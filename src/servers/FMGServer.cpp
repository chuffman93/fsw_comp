/*
* FMGServer.cpp
*
*  Last Modified: Jack Dinkel 12/16/16
*/

#include "servers/FMGServer.h"
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

namespace AllStar{
namespace Servers{

// ----- File Manager functions ------------------------------------------------------------------------------------
FileManager::FileManager(string path, string tlmType){
	file_path = path;
	TLM_type = tlmType;
	bytes_written = 0;
	file_open = false;
	file_name = "";
	file = NULL;
}

void FileManager::CloseFile(){
	FILE * fd;

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

void FileManager::MoveFile(){
	// Move the file out of CUR
	string new_file_name = file_name;
	new_file_name.erase(file_path.length(), 4);
	rename(file_name.c_str(), new_file_name.c_str());
	file_name = new_file_name;
}

void FileManager::OpenFile(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	file = fopen(file_name.c_str(), "a");
	if (file == NULL) { logger->Log(LOGGER_LEVEL_WARN, "Error opening file"); }
	file_open = true;
	bytes_written = 0;
}

void FileManager::GetFileName(){
	int time = getTimeInSec();
	int boot_count = 0;// TODO
	char fileName[100];
	sprintf(fileName, "%s/CUR/%s_%d_%d", file_path.c_str(), TLM_type.c_str(), boot_count, time);
	file_name = fileName;
}

void FileManager::OpenNewFile(){
	GetFileName();
	OpenFile();
}

void FileManager::Write(string buf, int buf_size){
	fwrite(buf.c_str(), sizeof(char), buf_size, file);
	fwrite("\n", sizeof(char), 1, file);
	fflush(file);
	bytes_written += buf_size + 1;
}

bool FileManager::Log(string buf){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	int buf_size = buf.size();

	if (buf_size >= fmgServer->FMGConfiguration.maxFileSize) { logger->Log(LOGGER_LEVEL_WARN, "Telemetry larger than file size"); }
	else{
		// Check if file is open
		if (!file_open) {
			logger->Log(LOGGER_LEVEL_DEBUG, "File not open");
			OpenNewFile();
		}

		if (bytes_written + buf_size + 1 < fmgServer->FMGConfiguration.maxFileSize) {
			Write(buf, buf_size);
		}
		else { // File is full, open new file and write to it
			CloseFile();
			OpenNewFile();
			Write(buf, buf_size);
		}
	}
	return true;
}

void LogFSS(void){
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	// Do SD cards work?
	// How full is each SD card?
}

// ----- FILServer functions ------------------------------------------------------------------------------------
FMGServer::FMGServer(string nameIn, LocationIDType idIn) :
					SubsystemServer(nameIn, idIn, 10, 1000),
					Singleton(),
					CMDLogger(CMD_FILE_PATH, "CMD"),
					ERRLogger(ERR_FILE_PATH, "ERR"),
					FSSLogger(FSS_FILE_PATH, "FSS"),
					GENLogger(GEN_FILE_PATH, "GEN"),
					HSTLogger(HST_FILE_PATH, "HST"),
					MODLogger(MOD_FILE_PATH, "MOD"),
					SSSLogger(SSS_FILE_PATH, "SSS"),
					SWPLogger(SWP_FILE_PATH, "SWP"),
					RADLogger(RAD_FILE_PATH, "RAD"),
					ACPLogger(ACP_FILE_PATH, "ACP"),
					resetReady(false),
					comReady(false),
					move_from_CUR(false)
					{
	FMGConfiguration.maxFileSize = 5000;
}

FMGServer::~FMGServer(){
}

void FMGServer::Initialize(void){

}

#ifdef TEST
void FMGServer::Destroy(void){

}
#endif

bool FMGServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

FMGServer & FMGServer::operator=(const FMGServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void FMGServer::CloseAndMoveAllFiles(){
	CMDLogger.CloseFile();
	ERRLogger.CloseFile();
	FSSLogger.CloseFile();
	GENLogger.CloseFile();
	HSTLogger.CloseFile();
	MODLogger.CloseFile();
	SSSLogger.CloseFile();
	SWPLogger.CloseFile();
    RADLogger.CloseFile();
    ACPLogger.CloseFile();
}

void FMGServer::Log(FILServerDestinationEnum dest, string buf){
	FilePacket packet;
	packet.buffer = buf;
	packet.dest = dest;
	FileQueue.push(packet);
}

void FMGServer::CallLog(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	string str;
	str = FileQueue.front().buffer;

	switch (FileQueue.front().dest){
	case DESTINATION_CMD:
		if ( !CMDLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Command File");
		}
		break;
	case DESTINATION_ERR:
		if ( !ERRLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Error File");
		}
		break;
	case DESTINATION_FSS:
		if ( !FSSLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to File System File");
		}
		break;
	case DESTINATION_GEN:
		if ( !GENLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to General File");
		}
		break;
	case DESTINATION_HST:
		if( !HSTLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Health and Status File");
		}
		break;
	case DESTINATION_MOD:
		if( !MODLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Mode File");
		}
		break;
	case DESTINATION_SSS:
		if ( !SSSLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Science System File");
		}
		break;
	case DESTINATION_SWP:
		if ( !SWPLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to HotSwap File");
		}
		break;
	case DESTINATION_RAD:
		if ( !RADLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to Rad File");
		}
		break;
	case DESTINATION_ACP:
		if ( !RADLogger.Log(str.c_str()) ) {
			logger->Log(LOGGER_LEVEL_WARN, "Error writing to ACP File");
		}
		break;
	default:
		logger->Log(LOGGER_LEVEL_WARN, "FMGServer: Unknown destination!");
		break;
	}

	FileQueue.pop();
}

void FMGServer::PrepVerboseHST(void){
	// tar the two most recent health and status files into verbose HST directory
	char cmd[256];
	sprintf(cmd, "tar -czf %s -C %s `ls -tr %s | head -2`", VERBOSE_HST_DIRECTORY, HST_FILE_PATH, HST_FILE_PATH);
	system(cmd);
}
// -----------------------------------------------------------------------------------------------------------------


// --------- State Machine -----------------------------------------------------------------------------------------
void FMGServer::loopInit(void){
	//nothing to initialize
	currentState = ST_RUN;
}

void FMGServer::loopRun(void) {
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	move_from_CUR = true;

	if (!FileQueue.empty()) {
		CallLog();
	}
	else if (modeManager->GetMode() == MODE_RESET) {
		currentState = ST_RESET;
	}
	else if (modeManager->GetMode() == MODE_COM){
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

	if (!FileQueue.empty()) {
		CallLog();
	}
	else if (modeManager->GetMode() != MODE_COM) {
		if (modeManager->GetMode() == MODE_RESET) {
			comReady = false;
			currentState = ST_RESET;
		}
		else {
			move_from_CUR = true;
			CloseAndMoveAllFiles();
			system(CLEAR_CUR_DIRECTORIES_SCRIPT);
			comReady = false;
			currentState = ST_RUN;
		}
	}
}

void FMGServer::loopReset(void){
	move_from_CUR = true;
	CloseAndMoveAllFiles();
	resetReady = true;
	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	resetReady = false;
	currentState = ST_RUN;
}

// -----------------------------------------------------------------------------------------------------------------




} // End of namespace Servers
} // End of namespace AllStar
