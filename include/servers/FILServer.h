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

#define GENERAL_FILE_LOCATION		"/SD_2/GENERAL/file.txt"
#define HEALTH_FILE_LOCATION		"/SD_2/HEALTH/file.txt"
#define MODE_FILE_LOCATION			"/SD_2/MODE/file.txt"
#define HOTSWAP_FILE_LOCATION		"/SD_2/HOTSWAP/file.txt"
#define ERROR_FILE_LOCATION			"/SD_2/ERROR/file.txt"
#define DIAGNOSTIC_FILE_LOCATION	"/SD_2/DIAGNOSTIC/file.txt"
#define RAD_FILE_LOCATION			"/SD_3/RAD/file.txt"


struct FilePacket {
	FILServerDestinationEnum dest;
	string buffer;
};

struct FileInfo {
	ofstream file;
	string file_name;
	int bytes_written;
};

class FILServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();
	// File Logging functions
	bool GeneralLog(string buf);
	bool HealthLog(string buf);
	bool ModeLog(string buf);
	bool HotSwapLog(string buf);
	bool ErrorLog(string buf);
	bool DiagnosticLog(string buf);
	bool RadLog(string buf); // Do we need this?

	// FileQueue
	queue<FilePacket> FileQueue;

private:
	static void Initialize(void);
	FileInfo GeneralInfo;
	FileInfo HealthInfo;
	FileInfo ModeInfo;
	FileInfo HotSwapInfo;
	FileInfo ErrorInfo;
	FileInfo DiagnosticInfo;
	FileInfo RadInfo;

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
