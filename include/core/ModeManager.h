/*
 * ModeManager.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef MODEMANAGER_H_
#define MODEMANAGER_H_

#include <set>

#include "core/ModeListener.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"

namespace AllStar
{
    namespace Core
    {
        class ModeManager : public Singleton
        {
			
			/*! \brief Declare Factory a friend class 
			*	
			*	This allows factory to call ModeManager's private constructor
			*/
			friend class Factory;
        public:

            bool Attach(ModeListener & listener);
            bool Detach(ModeListener & listener);

            SystemModeEnum GetMode(void);
            bool SetMode(SystemModeEnum newMode, LocationIDType server);

        protected:
            ModeManager(void);
            void NotifyAll(void);

        private:
			virtual ~ModeManager(void);
			static void Initialize(void);
#ifdef TEST
			static void Destroy(void);
#endif
			bool IsFullyInitialized(void);
		
            std::set<ModeListener *> listeners;

            static bool validTransition[MODE_NUM_MODES][MODE_NUM_MODES];

            SystemModeEnum mode;
        };
    }
}

#endif /* MODEMANAGER_H_ */
