#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include "core/ACPPacket.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"

#include <stdint.h>
#include <map>
#include <stdio.h>

namespace AllStar{
namespace Core{

#define DISPATCHER_QUEUE_LENGTH 		16

#define DISPATCHER_WAIT_TIME			(100000) //was 100 kernel ticks, 100 ms

#define DISPATCHER_MAX_DELAY 			(10000*1000) //was 10 kernel ticks, 10 ms in ns

#define DISPATCHER_MAX_RESPONSE_TRIES	20

#define MAX_PACKET_SIZE			65551

class Dispatcher : public Singleton {
	friend class Factory;

public:
	typedef enum DispatcherCheckEnum{
		CHECK_MATCHING_RESPONSE,
		CHECK_DEST_SOURCE,
		CHECK_SAME
	}DispatcherCheckType;

	// Unique number for each packet so that we can track them.
	static uint16 packetNumber;

	typedef bool (Dispatcher::* PacketCheckFunctionType)(const ACPPacket & packetIn, const ACPPacket & packetOut) const;

	bool Dispatch(ACPPacket & packet);

	inline bool IsPacketMatchingResponse(const ACPPacket & packetIn, const ACPPacket & packetOut) const;
	inline bool IsPacketDestMatchingSource(const ACPPacket & packetIn, const ACPPacket & packetOut) const;
	inline bool IsPacketSame(const ACPPacket & packetIn, const ACPPacket & packetOut) const;

	bool CheckQueueForMatchingPacket(const ACPPacket & packetIn,
									 ACPPacket * &packetOut,
									 DispatcherCheckType type);

	void CleanRXQueue(void);

	// --- Message queue for received messages ------------------------------
	mqd_t queueHandleRX;
	struct mq_attr queueAttrRX;
	int qInitRX;
	static char * queueNameRX;

private:
	bool IsFullyInitialized(void);

	static void * InvokeHandler(void * parameters);

	virtual ~Dispatcher(void );

	Dispatcher(void);

	Dispatcher & operator=(const Dispatcher & source);
};

} // End of namespace Core
} // End of namespace AllStar

#endif  //_DISPATCHER_H
