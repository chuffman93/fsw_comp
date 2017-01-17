/*
* Schedule Server for controlling what mode to switch to.
*/
#ifndef SCHSERVER_H_
#define SCHSERVER_H_

#include <list>

#include "servers/SubsystemServer.h"

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/MessageHandler.h"
#include "core/Singleton.h"
#include "util/ConfigManager.h"

namespace AllStar{
namespace Servers{

#define SCHEDULE_MAX_SIZE 50

#define SCH_CONFIG_FILE "SCHConfig"
#define SCH_SCHEDULE_FILE "/home/root/Schedule"

class SCHServer : public SubsystemServer, public AllStar::Core::Singleton
{
	friend class AllStar::Core::Factory;
	typedef struct{
		double latitude;
		double longitude;
		double radius;
		uint32_t timeoutms;
		SystemModeEnum mode;
	}SCHItem;

	typedef struct {
		uint8_t sizeOfDefaultSchedule;
		SCHItem defaultScheduleArray[SCHEDULE_MAX_SIZE];
	}SCHConfig;


public:
	void SubsystemLoop(void);

	bool RegisterHandlers();

	void LoadNextSchedule(void);

	void RequestCOMMode(void);

	void RequestReset(void);

private:

	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	SCHServer(std::string nameIn, LocationIDType idIn);
	~SCHServer();
	SCHServer & operator=(const SCHServer & source);

	util::ConfigManager<SCHConfig> configManager;

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	bool scheduleRunning;
	bool surpriseCOM;
	bool resetRequest;

	std::list<SCHItem> defaultSchedule;
	std::list<SCHItem> currentSchedule;
	std::list<SCHItem> nextSchedule;

	BEGIN_STATE_MAP
	END_STATE_MAP
};


} // End of namespace Servers
} // End of namespace AllStar

#endif /* SCHSERVER_H_ */
