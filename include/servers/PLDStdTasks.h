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

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::ReturnMessage * PLDHealthStatus(void);
		Phoenix::Core::ReturnMessage * PLDPicture(void);
		Phoenix::Core::ReturnMessage * PLDGetPicture(void);
		Phoenix::Core::ReturnMessage * PLDDataRequest(const uint32 &  packetNum);
		Phoenix::Core::ReturnMessage * PLDResSet(const uint32 & resX, const uint32 & resY);
		Phoenix::Core::ReturnMessage * PLDConfigChunkSize(const uint32 & chunkSize);
		Phoenix::Core::ReturnMessage * PLDSetGain(const uint32 & gain);
		Phoenix::Core::ReturnMessage * PLDSetExpTime(const uint32 & expTime);
		Phoenix::Core::ReturnMessage * PLDReset(void);
		bool PLDPictureProcess(Phoenix::Core::ReturnMessage * retMsg);
		Phoenix::Core::ReturnMessage * PLDGetData(void);
	}
}

#endif /* EPSSTDTASKS_H_ */
