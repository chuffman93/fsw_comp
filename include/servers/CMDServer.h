/*
 * CMDServer.h
 *
 *  Created on: Feb 12, 2013
 *      Author: Caitlyn
 *  Modified:
 */

#ifndef CMDSERVER_H_
#define CMDSERVER_H_

#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "util/FileHandler.h"

namespace AllStar{
namespace Servers{

class CMDServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	void SubsystemLoop(void);
	void Update(SystemModeEnum mode){}
	bool RegisterHandlers();
	static int subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	CMDServer(std::string nameIn, LocationIDType idIn);
	~CMDServer();
	CMDServer & operator=(const CMDServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	char * CMDFiles [5];

	BEGIN_STATE_MAP
	END_STATE_MAP
};

}
}

#endif /* CMDSERVER_H_ */
