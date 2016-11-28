/*
 * FILServer.h
 *
 *  Created on: Nov 28, 2016
 *      Author: Jack Dinkel
 *  Modified:
 */

#ifndef FILSERVER_H_
#define FILSERVER_H_

#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "util/FileHandler.h"

namespace AllStar{
namespace Servers{

class FILServer : public SubsystemServer, public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:
	bool RegisterHandlers();

private:
	static void Initialize(void);

#ifdef TEST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	FILServer(std::string nameIn, LocationIDType idIn);
	~FILServer();
	FILServer & operator=(const FILServer & source);

	// Member variables needed to register message handlers.
	AllStar::Core::MessageHandlerRegistry reg;
	AllStar::Core::Arbitrator arby;

	// Modes
	void loopInit();;

	BEGIN_STATE_MAP
	STATE_MAP_ENTRY(&FILServer::loopInit)
	END_STATE_MAP

};

}
}

#endif /* FILSERVER_H_ */
