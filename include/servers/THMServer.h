/*
* THMServer.h
*
*  Created on: July 09, 2012
*      Author: Caitlyn Cooke
*/

#ifndef THMSERVER_H_
#define THMSERVER_H_



#include "servers/SubsystemServer.h"

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"

#define PULL_INTERVAL 10000
#define ERROR_CHECK_MULTIPLIER 2


namespace Phoenix
{
	namespace Servers
	{	
		enum THMInfoEnum
		{
			THM_BUS_ZERO = 0,
			THM_BUS_ONE,
			THM_BUS_TWO,
			THM_BUS_THREE,
			
			THM_COM_MODE = 0,
			THM_DEFAULT_MODE,
			
			THM_COLD = 0,
			THM_HOT
		};
		
		//number of temperature sensors to check to see if we are in ecplipse
		//check BUS2:ALL, BUS3:1, 2, 3, 7, 8, 9
		#define THERMAL_SENSORS_EXO_CHECK			22
		
		class THMServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call THMServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
     		public:
			
			void SubsystemLoop(void);
			void Update(const Core::SystemMode * mode);
			
			uint16* getUpper(void);	
			uint16* getLower(void);	
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
			bool RegisterHandlers(void);

		private:
			/*! \brief Initialize the THMServer Class
			*
			*  Initializes the operating system constructs needed for
			*  THMServer to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for THMServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef HOST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if THMServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
		
			void THMComMode(Phoenix::Core::ModeManager * modeManager);
			void THMDefaultMode(Phoenix::Core::ModeManager * modeManager);
				
			THMServer(std::string nameIn, LocationIDType idIn);
			~THMServer();
			THMServer & operator=(const THMServer & source);
			uint16 UBOUND[16];
			uint16 LBOUND[16];			
			
			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;
		};
	}
}

#endif /* THMSERVER_H_ */
