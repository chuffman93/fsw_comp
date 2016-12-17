/*
 * FMGServer.h
 *
 *  Last Modified: Jack Dinkel 12/16/16
 */

#ifndef FMGSERVER_H_
#define FMGSERVER_H_

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

#define GEN_FILE_PATH		"/SD_2/GEN"
#define HST_FILE_PATH		"/SD_2/HST"
#define MOD_FILE_PATH		"/SD_2/MOD"
#define SWP_FILE_PATH		"/SD_2/SWP"
#define ERR_FILE_PATH		"/SD_2/ERR"
#define DGN_FILE_PATH		"/SD_2/DGN"
#define FSS_FILE_PATH		"/SD_2/FSS"
#define SSS_FILE_PATH		"/SD_2/SSS"
#define RAD_FILE_PATH		"/SD_3/RAD"

#define MAX_FILE_SIZE		500 // There is also an old enum to deal with this


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

class FMGServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

	// FileQueue
	queue<FilePacket> FileQueue;

private:
	static void Initialize(void);
	FileManager GENLogger;
	FileManager HSTLogger;
	FileManager MODLogger;
	FileManager SWPLogger;
	FileManager ERRLogger;
	FileManager DGNLogger;
	FileManager FSSLogger;
	FileManager SSSLogger;
	FileManager RADLogger;

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	FMGServer(std::string nameIn, LocationIDType idIn);
	~FMGServer();
	FMGServer & operator=(const FMGServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	// Modes
	void loopInit();;

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&FMGServer::loopInit)
	END_STATE_MAP

};

}
}

#endif /* FMGSERVER_H_ */
