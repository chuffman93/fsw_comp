/*
 * FILServer.h
 *
 *  Created on: Nov 28, 2016
 *      Author: Jack Dinkel
 *  Modified:
 */

#ifndef FILSERVER_H_
#define FILSERVER_H_

#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "util/FileHandler.h"
#include <queue>

namespace AllStar{
namespace Servers{

#define GENERAL_FILE_PATH		"/SD_2/GENERAL"
#define HEALTH_FILE_PATH		"/SD_2/HEALTH"
#define MODE_FILE_PATH			"/SD_2/MODE"
#define HOTSWAP_FILE_PATH		"/SD_2/HOTSWAP"
#define ERROR_FILE_PATH			"/SD_2/ERROR"
#define DIAGNOSTIC_FILE_PATH	"/SD_2/DIAGNOSTIC"
#define RAD_FILE_PATH			"/SD_3/RAD"

#define MAX_FILE_SIZE				150


struct FilePacket {
	FILServerDestinationEnum dest;
	string buffer;
};

class FileManager{
public:
	FileManager(string path);
	void CloseFile();
	void OpenFile();
	void GetFileName();
	void OpenNewFile();
	void Write(string buf, int buf_size);
	bool Log(string buf);
private:
	FILE * file;
	string file_path;
	string file_name;
	int bytes_written;
	bool file_open;
};

class FILServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

	// FileQueue
	queue<FilePacket> FileQueue;

private:
	static void Initialize(void);
	FileManager GeneralLogger;
	FileManager HealthLogger;
	FileManager ModeLogger;
	FileManager HotSwapLogger;
	FileManager ErrorLogger;
	FileManager DiagnosticLogger;
	FileManager RadLogger;

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	FILServer(std::string nameIn, LocationIDType idIn);
	~FILServer();
	FILServer & operator=(const FILServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	// Modes
	void loopInit();;

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&FILServer::loopInit)
	END_STATE_MAP

};

}
}

#endif /* FILSERVER_H_ */
