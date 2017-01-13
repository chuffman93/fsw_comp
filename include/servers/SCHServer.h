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
	/*! \brief Declare Factory a friend class
	*
	*	This allows factory to call SCHServer's private constructor
	*/
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

	/*! \brief Registers this server's message handlers.
	*
	*  \note Dispatcher must be initialized before this
	*      function can be called.
	*
	*  Registers any message handlers with the member
	*  MessageHandlerRegistry and modifies permissions
	*  with the member Arbitrator. Then those two members
	*  are added to the Dispatcher.
	*/
	bool RegisterHandlers();
	SCHItem GetSCHItem(unsigned int n);
	void LoadDefaultSchedule(void);
	SCHItem ParseLine(string line);
	int LoadNextSchedule(void);

private:
	/*! \brief Initialize the SCHServer Class
	*
	*  Initializes the operating system constructs needed for
	*  PLDServer to work properly.
	*/
	static void Initialize(void);

	/*! \brief Static Destructor for SCHServer
	*
	*  Frees all internal memory use, frees all operating system
	*  objects used, and deletes the singleton instance pointer.
	*/
#ifdef TEST
	static void Destroy(void);
#endif

	/*! \brief Checks if SCHServer Class is initialized
	*
	*  \return true if the initialization was successful and
	*  false otherwise.
	*/
	bool IsFullyInitialized(void);

	SCHServer(std::string nameIn, LocationIDType idIn);
	~SCHServer();
	SCHServer & operator=(const SCHServer & source);

	util::ConfigManager<SCHConfig> configManager;

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	bool scheduleRunning;

	/// \brief The vectors of mode schedules and payload schedules.
	std::list<SCHItem> defaultSchedule;
	std::list<SCHItem> currentSchedule;
	std::list<SCHItem> nextSchedule;

	BEGIN_STATE_MAP
	END_STATE_MAP
};


} // End of namespace Servers
} // End of namespace AllStar

#endif /* SCHSERVER_H_ */
