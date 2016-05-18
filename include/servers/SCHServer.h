/*
* Schedule Server for controlling what mode to switch to.
*/
#ifndef SCHSERVER_H_
#define SCHSERVER_H_

#include <list>

#include "servers/SubsystemServer.h"
#include "servers/SCHMode.h"
#include "servers/SCHEvent.h"
#include "servers/SCHHandlers.h"
#include "servers/SCHStdTasks.h"

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/MessageHandler.h"
#include "core/Singleton.h"
#include "core/VariableTypeEnum.h"

namespace Phoenix
{
	namespace Servers
	{
		// Lat, Lon, Alt (GPS format?) m
		//From Nicole
		#define GROUND_STATION_X		-1280891
		#define GROUND_STATION_Y		-4702971
		#define GROUND_STATION_Z		4110321
		
		//Real one
// 		#define GROUND_STATION_X		-1278064.576544036
// 		#define GROUND_STATION_Y		-4699892.870631095
// 		#define GROUND_STATION_Z		4106659.631408538
		
		#define DEFAULT_GROUND_STATION_RANGE	1500000

		class SCHServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call SCHServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
		public:
			
			void SubsystemLoop(void);
			void Update(const Core::SystemMode * mode);

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

			bool SetCurrentSchedule(std::list<SCHMode *> * newSchedule);
			bool SetNextSchedule(std::list<SCHMode *> * newSchedule);
			bool SetCurrentPLDSchedule(std::list<SCHEvent *> * newPLDSchedule);
			bool SetNextPLDSchedule(std::list<SCHEvent *> * newPLDSchedule);
			bool RunCurrentSchedule(void);
			bool SetNewDefaultRange(const float & newRange);

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
#ifdef HOST
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
			
			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			bool scheduleRunning;
			
			float defaultScheduleRange;

			/// \brief The vectors of mode schedules and payload schedules.
			std::list<SCHMode *> * currentSchedule;
			std::list<SCHMode *> * nextSchedule;

			std::list<SCHEvent *> * currentPLDSchedule;
			std::list<SCHEvent *> * nextPLDSchedule;
			
			std::list<SCHMode *> * defaultSchedule;
		};
	}
}

#endif /* SCHSERVER_H_ */
