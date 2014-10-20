
#include "servers/SCHServer.h"
#include "servers/SCHEvent.h"
#include "servers/SCHMode.h"
#include "servers/SCHModePosition.h"
#include "servers/SCHStdTasks.h"
#include "servers/SCHHandlers.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/PayloadPriorityMode.h"
#include "core/SystemMode.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		static SCHScheduleHandler * schScheduleHandler;
		static SCHPldScheduleHandler * schPldScheduleHandler;
		static SCHRunScheduleHandler * schRunScheduleHandler;
		static SCHDefaultRangeHandler * schDefaultRangeHandler;
		
		SCHServer::SCHServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			scheduleRunning = false;
			defaultScheduleRange = DEFAULT_GROUND_STATION_RANGE;
			defaultSchedule = new std::list<SCHMode *>;
			defaultSchedule->push_back(new SCHModePosition(MODE_COM, defaultScheduleRange, GROUND_STATION_X, GROUND_STATION_Y,
				GROUND_STATION_Z, true));
			defaultSchedule->push_back(new SCHModePosition(MODE_BUS_PRIORITY, defaultScheduleRange, GROUND_STATION_X, 
				GROUND_STATION_Y, GROUND_STATION_Z, false));
			currentSchedule = NULL;
			nextSchedule = NULL;
			currentPLDSchedule = NULL;
			nextPLDSchedule = NULL;
		}

		SCHServer::~SCHServer()
		{
			// Left Intentionally Blank
		}

		SCHServer & SCHServer::operator=(const SCHServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

 			return *this;
		}
		
		void SCHServer::Initialize(void)
		{
			schScheduleHandler = new SCHScheduleHandler();
			schPldScheduleHandler = new SCHPldScheduleHandler();
			schRunScheduleHandler = new SCHRunScheduleHandler();
			schDefaultRangeHandler = new SCHDefaultRangeHandler();
		}
		
#ifdef HOST
		void SCHServer::Destroy(void)
		{
			delete schScheduleHandler;
			delete schPldScheduleHandler;
			delete schRunScheduleHandler;
			delete schDefaultRangeHandler;
		}
#endif
		
		bool SCHServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void SCHServer::Update(const SystemMode * mode)
		{
			//update mode stuff!
		}
		
		bool SCHServer::RegisterHandlers()
		{
			volatile bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, SCH_BUILD_SCHEDULE_CMD), schScheduleHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, SCH_BUILD_PLD_SCHEDULE_CMD), schPldScheduleHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, SCH_RUN_SCHEDULE_CMD), schRunScheduleHandler);
			
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, SCH_DEFAULT_RANGE_CONFIG), schDefaultRangeHandler);
			
			success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, SCH_BUILD_SCHEDULE_CMD), true);
			success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, SCH_BUILD_PLD_SCHEDULE_CMD), true);
			success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, SCH_RUN_SCHEDULE_CMD), true);
			
			success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_CONFIG, SCH_DEFAULT_RANGE_CONFIG), true);

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}
		
		void SCHServer::SubsystemLoop(void)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));			
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

			const SystemMode * mode;
			while(1)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));
				
				
				while(scheduleRunning)
				{
					//debug_led_set_led(4, LED_ON);
					if(currentSchedule == NULL)
					{
						scheduleRunning = false;
						break;
					}
					while(!(currentSchedule->empty()))
					{
						// Kick the dog?
						uint64_t LastWakeTimeInner = getTimeInMilis();
						//wdm->Kick();
						while(dispatcher->Listen(id));
						/*debug_led_set_led(2, LED_TOGGLE);*/
						
						if(currentSchedule->front()->CheckGoToNextMode())
						{
							currentSchedule->pop_front();
						}
						// Check current mode
						mode = modeManager->GetMode();
						// Payload schedule
						if((mode == PayloadPriorityMode::GetInstance()) && (!(currentPLDSchedule == NULL)))
						{
							if((!(currentPLDSchedule->empty())) && (currentPLDSchedule->front()->CheckGoToNextEvent()))
							{
								currentPLDSchedule->pop_front();
								if(currentPLDSchedule->empty())
								{
									delete currentPLDSchedule;
									if(nextPLDSchedule != NULL)
									{
										if(!(nextPLDSchedule->empty()))
										{
											currentPLDSchedule = nextPLDSchedule;
											nextPLDSchedule = NULL;
										}
									}
								}
							}
						}
						waitUntil(LastWakeTimeInner, 1000);
					}
					delete currentSchedule;
					if(nextSchedule != NULL)
					{
						if(!(nextSchedule->empty()))
						{
							currentSchedule = nextSchedule;
							nextSchedule = NULL;
						}
					}
					scheduleRunning = false;
				}
				//debug_led_set_led(2, LED_TOGGLE);
				if(defaultSchedule == NULL)
				{
					defaultSchedule = new std::list<SCHMode *>;
					defaultSchedule->push_back(new SCHModePosition(MODE_COM, defaultScheduleRange, GROUND_STATION_X, GROUND_STATION_Y,
						GROUND_STATION_Z, true));
					defaultSchedule->push_back(new SCHModePosition(MODE_BUS_PRIORITY, defaultScheduleRange, GROUND_STATION_X,
						GROUND_STATION_Y, GROUND_STATION_Z, false));
				}
				if(defaultSchedule != NULL)
				{
					if(!defaultSchedule->empty())
					{
						if(defaultSchedule->front()->CheckGoToNextMode())
						{
							defaultSchedule->pop_front();
						}
					}
					else
					{
						delete defaultSchedule;
						defaultSchedule = NULL;
					}
				}
				waitUntil(LastWakeTime, 1000);
			}
		}

		bool SCHServer::SetCurrentSchedule(list<SCHMode *> * newSchedule)
		{
			if((currentSchedule != NULL) || (scheduleRunning))
			{
				//schedule is not complete fail
				return false;
			}
			currentSchedule = newSchedule;
 			return true;
		}

		bool SCHServer::SetNextSchedule(list<SCHMode *> * newSchedule)
		{
			if((nextSchedule != NULL))
			{
				//schedule is not complete fail
				return false;
			}
			nextSchedule = newSchedule;
 			return true;
		}

		bool SCHServer::SetCurrentPLDSchedule(list<SCHEvent *> * newPLDSchedule)
		{
			if(currentPLDSchedule != NULL)
			{
				//PLD schedule is not complete fail
				return false;
			}
			currentPLDSchedule = newPLDSchedule;
 			return true;
		}

		bool SCHServer::SetNextPLDSchedule(list<SCHEvent *> * newPLDSchedule)
		{
			if(nextPLDSchedule != NULL)
			{
				//PLD schedule is not complete fail
				return false;
			}
			nextPLDSchedule = newPLDSchedule;
 			return true;
		}

		bool SCHServer::RunCurrentSchedule()
		{
			if(currentSchedule == NULL)
			{
				return false;
			}
			if(currentSchedule->empty())
			{
				//schedule doesn't exist fail
				return false;
			}
			if(scheduleRunning)
			{
				// schedule is already running
				return false;
			}
			scheduleRunning = true;
 			return true;
		}
		
		bool SCHServer::SetNewDefaultRange(const float & newRange)
		{
			defaultScheduleRange = newRange;
			return true;
		}
	}
}
