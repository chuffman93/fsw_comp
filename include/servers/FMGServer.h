/*
 * FMGServer.h
 *
 *  Last Modified: Jack Dinkel 12/16/16
 */

#ifndef FMGSERVER_H_
#define FMGSERVER_H_

#include "servers/SubsystemServer.h"
#include "servers/Structs.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "servers/CMDServer.h"
#include <queue>

namespace AllStar{
namespace Servers{

#define CLEAR_CUR_DIRECTORIES_SCRIPT "/home/root/scripts/clear_CUR_directories.sh"

// TODO: Do we want to be able to set this from the ground?
#define MAX_FILE_SIZE		5000 // 5 kB

void LogFSS(void);

struct FilePacket {
	FILServerDestinationEnum dest;
	uint8 * buffer;
	size_t size;
};

class FileManager{
public:
	FileManager(string path, string tlmType);
	void CloseFile(void);
	void MoveFile(void);
	void OpenFile(void);
	void GetFileName(void);
	void OpenNewFile(void);
	void Write(uint8 * buf, std::size_t buf_size);
	bool Log(uint8 * buf, std::size_t size);
private:
	string TLM_type;
	FILE * file;
	string file_path;
	string file_name;
	int bytes_written;
	bool file_open;
};

class FMGServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	void Log(FILServerDestinationEnum dest, uint8 * buf, size_t size);


	bool isResetReady() const {
		return resetReady;
	}

	void setResetReady(bool comReady) {
		this->resetReady = comReady;
	}

	bool checkMoveFromCur() const {
		return move_from_CUR;
	}

	void setMoveFromCur(bool moveFromCur) {
		move_from_CUR = moveFromCur;
	}

	bool isComReady() const {
		return comReady;
	}

	void setComReady(bool comReady) {
		this->comReady = comReady;
	}

	// FileQueue
	queue<FilePacket> FileQueue;

	uint16 loadBootCount();
	void bootConfig(); // read in configs on bootup
	bool updateConfig(); // update configs after COM pass

	static FMGConfig FMGConfiguration;
	static uint16 bootCount;

private:
	FileManager CMDLogger;
	FileManager ERRLogger;
	FileManager GENLogger;
	FileManager GPSLogger;
	FileManager MODLogger;
	FileManager PWRLogger;
	FileManager SPILogger;
	FileManager SWPLogger;
	FileManager THMLogger;
	FileManager ACS_HSTLogger;
	FileManager CDH_HSTLogger;
	FileManager COM_HSTLogger;
	FileManager EPS_HSTLogger;
	FileManager PLD_HSTLogger;

	bool IsFullyInitialized(void);

	FMGServer(std::string nameIn, LocationIDType idIn);
	~FMGServer();
	FMGServer & operator=(const FMGServer & source);

	bool resetReady;
	bool move_from_CUR;
	bool comReady;
	void CloseAndMoveAllFiles(void);
	void CallLog(void);
	void PrepVerboseHST(void);

	// Modes
	void loopInit();
	void loopRun();
	void loopComPrep();
	void loopCom();
	void loopReset();

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&FMGServer::loopInit)
	STATE_MAP_ENTRY(&FMGServer::loopRun)
	STATE_MAP_ENTRY(&FMGServer::loopComPrep)
	STATE_MAP_ENTRY(&FMGServer::loopCom)
	STATE_MAP_ENTRY(&FMGServer::loopReset)
	END_STATE_MAP

	enum FMG_States{
		ST_INIT = 0,
		ST_RUN,
		ST_COM_PREP,
		ST_COM,
		ST_RESET
	};
};

}
}

#endif /* FMGSERVER_H_ */
