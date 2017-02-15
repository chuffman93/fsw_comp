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

	// Close the file
	fclose(file);
	file_open = false;

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
	sprintf(fileName, "%s/CUR/GEN_%d_%d", file_path.c_str(), boot_count, time); // TODO!!
	file_name = fileName;
}

void FileManager::OpenNewFile(){
	GetFileName();
	OpenFile();
}

void FileManager::Write(uint8 * buf, size_t buf_size){
	fwrite(buf, sizeof(char), buf_size, file);
	fflush(file);
	bytes_written += buf_size + 1;
}

bool FileManager::Log(uint8 * buf, size_t buf_size){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

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

void LogFSS(void){
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	// Do SD cards work?
	// How full is each SD card?
}

// ----- FILServer functions ------------------------------------------------------------------------------------
FMGServer::FMGServer(string nameIn, LocationIDType idIn) :
					SubsystemServer(nameIn, idIn, 10, 1000),
					Singleton(),
					arby(idIn),
					ACPLogger(ACP_FILE_PATH),
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

void FMGServer::Log(FILServerDestinationEnum dest, uint8 * buf, size_t size){
	FilePacket packet;
	packet.buffer = buf;
	packet.size = size;
	packet.dest = dest;
	FileQueue.push(packet);
}
// -----------------------------------------------------------------------------------------------------------------


// --------- State Machine -----------------------------------------------------------------------------------------
void FMGServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (!FileQueue.empty()){
		uint8 * buf = FileQueue.front().buffer;
		size_t size = FileQueue.front().size;

		switch (FileQueue.front().dest){
		case DESTINATION_ACP:
			if ( !ACPLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to ACP File");
			}
			break;
		case DESTINATION_GEN:
			if ( !GENLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to General File");
			}
			break;
		case DESTINATION_HST:
			if( !HSTLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Health File");
			}
			break;
		case DESTINATION_MOD:
			if( !MODLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Mode File");
			}
			break;
		case DESTINATION_SWP:
			if ( !SWPLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to HotSwap File");
			}
			break;
		case DESTINATION_ERR:
			if ( !ERRLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Error File");
			}
			break;
		case DESTINATION_DGN:
			if ( !DGNLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Diagnostic File");
			}
			break;
		case DESTINATION_FSS:
			if ( !FSSLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to File System File");
			}
			break;
		case DESTINATION_SSS:
			if ( !SSSLogger.Log(buf, size) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Science System File");
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
}

// -----------------------------------------------------------------------------------------------------------------




} // End of namespace Servers
} // End of namespace AllStar
