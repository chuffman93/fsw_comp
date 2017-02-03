/*
* Schedule Server for controlling what mode to switch to.
*/
#ifndef SCHSERVER_H_
#define SCHSERVER_H_

#include <list>

#include "servers/SubsystemServer.h"

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
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



public:
	struct SCHItem{
		double latitude;
		double longitude;
		double radius;
		bool enter_mode;
		int32 timeout;
		SystemModeEnum mode;
		int32 duration;
	};

	struct SCHConfig{
		uint8_t sizeOfDefaultSchedule;
		SCHItem defaultScheduleArray[SCHEDULE_MAX_SIZE];
	};

	void EnterNextMode(void);
	void EnterBusMode(void);
	int LoadDefaultScheduleConfigurations(void);
	void SubsystemLoop(void);

	bool RegisterHandlers();

	void RequestCOMMode(void);

	void RequestReset(void);

	void LoadDefaultSchedule(void);

	SCHItem ParseLine(string line);

	int LoadNextSchedule(void);

	/// \brief The vectors of mode schedules and payload schedules.
	std::list<SCHItem> defaultSchedule;
	std::list<SCHItem> currentSchedule;

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

	bool surpriseCOM;
	bool resetRequest;
	int64 lastWakeTime;
	int32 modeEnterTime;
	int32 lastBusEnter;
	bool itemEntered;

	BEGIN_STATE_MAP
	END_STATE_MAP
};


} // End of namespace Servers
} // End of namespace AllStar

#endif /* SCHSERVER_H_ */
