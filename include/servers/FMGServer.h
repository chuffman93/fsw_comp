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

#define CMD_FILE_PATH		"/SD_2/CMD"
#define DGN_FILE_PATH		"/SD_2/DGN"
#define ERR_FILE_PATH		"/SD_2/ERR"
#define GEN_FILE_PATH		"/SD_2/GEN"
#define FSS_FILE_PATH		"/SD_2/FSS"
#define HST_FILE_PATH		"/SD_2/HST"
#define MOD_FILE_PATH		"/SD_2/MOD"
#define SSS_FILE_PATH		"/SD_2/SSS"
#define SWP_FILE_PATH		"/SD_2/SWP"
#define RAD_FILE_PATH		"/SD_3/RAD"

#define CLEAR_CUR_DIRECTORIES_SCRIPT "/home/root/scripts/clear_CUR_directories.sh"

// TODO: Do we want to be able to set this from the ground?
#define MAX_FILE_SIZE		500 // There is also an old enum to deal with this


struct FilePacket {
	FILServerDestinationEnum dest;
	string buffer;
};

class FileManager{
public:
	FileManager(string path);
	void CloseFile(void);
	void FileManager::MoveFile(void);
	void OpenFile(void);
	void GetFileName(void);
	void OpenNewFile(void);
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
	void Log(FILServerDestinationEnum dest, string buf);

	// FileQueue
	queue<FilePacket> FileQueue;
	bool resetReady;

private:
	static void Initialize(void);
	FileManager CMDLogger;
	FileManager DGNLogger;
	FileManager ERRLogger;
	FileManager FSSLogger;
	FileManager GENLogger;
	FileManager HSTLogger;
	FileManager MODLogger;
	FileManager SSSLogger;
	FileManager SWPLogger;
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

	bool move_from_CUR;
	void CloseAndMoveAllFiles(void);
	void CallLog(void);

	// Modes
	void loopInit();
	void loopRun();
	void loopCom();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&FMGServer::loopInit)
	STATE_MAP_ENTRY(&FMGServer::loopRun)
	STATE_MAP_ENTRY(&FMGServer::loopCom)
	STATE_MAP_ENTRY(&FMGServer::loopReset)
	END_STATE_MAP

	enum FMG_States{
		ST_INIT = 0,
		ST_RUN,
		ST_COM,
		ST_RESET
	};
};

}
}

#endif /* FMGSERVER_H_ */
