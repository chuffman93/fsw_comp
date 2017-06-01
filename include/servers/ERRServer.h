#ifndef ERRSERVER_H_
#define ERRSERVER_H_

#include "servers/SubsystemServer.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"
#include <queue>

namespace AllStar {
namespace Servers {

class ERRServer : public SubsystemServer, public AllStar::Core::Singleton {
	friend class AllStar::Core::Factory;

public:
	void SubsystemLoop(void);

	void SendError(ErrorOpcodeType error);

private:
	bool IsFullyInitialized(void);

	ERRServer(std::string nameIn, LocationIDType idIn);

	~ERRServer();

	ERRServer & operator=(const ERRServer & source);

	std::queue<ErrorOpcodeType> ErrorQueue;

	BEGIN_STATE_MAP
	END_STATE_MAP
};


} // End of namespace Servers
} // End of namespace AllStar

#endif // ERRSERVER_H_
