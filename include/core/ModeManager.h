/*
 * ModeManager.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#ifndef MODEMANAGER_H_
#define MODEMANAGER_H_

#include "core/StdTypes.h"
#include "core/Singleton.h"
#include <set>

namespace AllStar{
namespace Core{

class ModeManager : public Singleton{
	friend class Factory;
	public:
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

		static bool validTransition[MODE_NUM_MODES][MODE_NUM_MODES];
		SystemModeEnum mode;
};

}
}

#endif // MODEMANAGER_H_
