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
	void SortError(ErrorOpcodeType error);

	void ACSError(ErrorOpcodeType error);
	void CDHError(ErrorOpcodeType error);
	void CMDError(ErrorOpcodeType error);
	void COMError(ErrorOpcodeType error);
	void EPSError(ErrorOpcodeType error);
	void FMGError(ErrorOpcodeType error);
	void GPSError(ErrorOpcodeType error);
	void PLDError(ErrorOpcodeType error);
	void SCHError(ErrorOpcodeType error);
	void GENError(ErrorOpcodeType error);

	// counters and flags
	uint8 ACSNotAliveCount;
	uint8 COMNotAliveCount;
	uint8 EPSNotAliveCount;
	uint8 PLDNotAliveCount;

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
