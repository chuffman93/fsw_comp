/*
 * EPSStdTasks.h
 * A collection of functions to be run in EPSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef EPSSTDTASKS_H_
#define EPSSTDTASKS_H_

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "servers/PLDServer.h"
#include "servers/SubsystemServer.h"

namespace AllStar
{
	namespace Servers
	{
		AllStar::Core::FSWPacket * PLDHealthStatus(void);
		AllStar::Core::FSWPacket * PLDPicture(void);
		AllStar::Core::FSWPacket * PLDGetPicture(void);
		AllStar::Core::FSWPacket * PLDDataRequest(const uint32 &  packetNum);
		AllStar::Core::FSWPacket * PLDResSet(const uint32 & resX, const uint32 & resY);
		AllStar::Core::FSWPacket * PLDConfigChunkSize(const uint32 & chunkSize);
		AllStar::Core::FSWPacket * PLDSetGain(const uint32 & gain);
		AllStar::Core::FSWPacket * PLDSetExpTime(const uint32 & expTime);
		AllStar::Core::FSWPacket * PLDReset(void);
		bool PLDPictureProcess(AllStar::Core::ReturnMessage * retMsg);
		AllStar::Core::FSWPacket * PLDGetData(void);
	}
}

#endif /* EPSSTDTASKS_H_ */
