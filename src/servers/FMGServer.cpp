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
FileManager::FileManager(string path){
	file_path = path;
	bytes_written = 0;
	file_open = false;
}

void FileManager::CloseFile(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	// Close the file
	fclose(file);
	file_open = false;

	// Compress the file and move it out of CUR
	string new_file_name = file_name;
	new_file_name.erase(file_path.length(), 4);
	char tar_cmd[128];
	sprintf(tar_cmd, "tar -czf %s %s", new_file_name.c_str(), file_name.c_str());
	if(!(fd = popen(tar_cmd, "r"))){
		logger->Log(LOGGER_LEVEL_ERROR, "FileManager: Error with tar command");
		// TODO: Needs much better error checking!
	}
	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "FileManager: Error closing file stream");
	}
	remove(file_name.c_str());

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
	sprintf(fileName, "%s/CUR/GEN_%d_%d", file_path.c_str(), boot_count, time);
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
	int buf_size = buf.size();

	if (buf_size >= MAX_FILE_SIZE) { logger->Log(LOGGER_LEVEL_WARN, "Telemetry larger than file size"); }
	else{
		// Check if file is open
		if (!file_open) {
			logger->Log(LOGGER_LEVEL_DEBUG, "File not open");
			OpenNewFile();
		}

		if (bytes_written + buf_size + 1 < MAX_FILE_SIZE) {
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


// ----- FILServer functions ------------------------------------------------------------------------------------
FMGServer::FMGServer(string nameIn, LocationIDType idIn) :
					SubsystemServer(nameIn, idIn, 10, 1000),
					Singleton(),
					arby(idIn),
					GENLogger(GEN_FILE_PATH),
					HSTLogger(HST_FILE_PATH),
					MODLogger(MOD_FILE_PATH),
					SWPLogger(SWP_FILE_PATH),
					ERRLogger(ERR_FILE_PATH),
					DGNLogger(DGN_FILE_PATH),
					FSSLogger(FSS_FILE_PATH),
					SSSLogger(SSS_FILE_PATH),
					RADLogger(RAD_FILE_PATH)
					{
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

bool FMGServer::RegisterHandlers(){
	bool success = true;
	return success;
}
// -----------------------------------------------------------------------------------------------------------------


// --------- State Machine -----------------------------------------------------------------------------------------
void FMGServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (!FileQueue.empty()){
		string str;
		str = FileQueue.front().buffer;

		switch (FileQueue.front().dest){
		case DESTINATION_GEN:
			if ( !GENLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to General File");
			}
			break;
		case DESTINATION_HST:
			if( !HSTLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Health File");
			}
			break;
		case DESTINATION_MOD:
			if( !MODLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Mode File");
			}
			break;
		case DESTINATION_SWP:
			if ( !SWPLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to HotSwap File");
			}
			break;
		case DESTINATION_ERR:
			if ( !ERRLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Error File");
			}
			break;
		case DESTINATION_DGN:
			if ( !DGNLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Diagnostic File");
			}
			break;
		case DESTINATION_FSS:
			if ( !FSSLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to File System File");
			}
			break;
		case DESTINATION_SSS:
			if ( !SSSLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Science System File");
			}
			break;
		case DESTINATION_RAD:
			if ( !RADLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Rad File");
			}
			break;
		default:
			logger->Log(LOGGER_LEVEL_WARN, "FMGServer: Unknown destination!");
			break;
		}

		FileQueue.pop();
	}
}

// -----------------------------------------------------------------------------------------------------------------




} // End of namespace Servers
} // End of namespace AllStar
