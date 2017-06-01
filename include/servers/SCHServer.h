/*
* Schedule Server for controlling what mode to switch to.
*/
#ifndef SCHSERVER_H_
#define SCHSERVER_H_

#include <list>

#include "servers/SubsystemServer.h"
#include "servers/Structs.h"
#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "util/ConfigManager.h"

namespace AllStar{
namespace Servers{

#define SCHEDULE_MAX_SIZE		8
#define DEFAULT_COM_DURATION	90

class SCHServer : public SubsystemServer, public AllStar::Core::Singleton {
	friend class AllStar::Core::Factory;

public:
	bool LoadDefaultScheduleConfigurations(char * filename);

	void UpdateDefaultSchedule(int numItems);

	void SetDefaultBusSchedule(void);

	void SubsystemLoop(void);

	void RequestCOMMode(void);

	void RequestReset(void);

	void LoadDefaultSchedule(void);

	void UpdateSchedule(int numItems);

	int LoadNextSchedule(void);

	SCHItem ScheduleArray[SCHEDULE_MAX_SIZE];
	SCHItem DefaultArray[SCHEDULE_MAX_SIZE];

	/// \brief The vectors of mode schedules and payload schedules.
	std::list<SCHItem> defaultSchedule;
	std::list<SCHItem> currentSchedule;

private:
	bool IsFullyInitialized(void);

	SCHServer(std::string nameIn, LocationIDType idIn);

	~SCHServer();

	SCHServer & operator=(const SCHServer & source);

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
