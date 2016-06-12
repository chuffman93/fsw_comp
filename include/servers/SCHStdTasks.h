/*
 * SCHStdTasks.h
 *
 * Created: 11/7/2013 11:00:28 AM
 *  Author: cypa8074
 */ 


#ifndef SCHSTDTASKS_H_
#define SCHSTDTASKS_H_

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{
		//build a schedule from file.
		Phoenix::Core::FSWPacket * BuildSchedule(const char * fileName);

		//run the current schedule loaded in the schedule server
		Phoenix::Core::FSWPacket * RunSchedule(void);

		//build a schedule from file.
		Phoenix::Core::FSWPacket * BuildPLDSchedule(const char * fileName);
		
		//Check if we are in-range to target
		bool InRange(const float & compareRange, const float & targetX, const float & targetY, const float & targetZ,
			const float & currentX, const float & currentY, const float & currentZ, const bool & lessThanComparison);
			
		Phoenix::Core::FSWPacket * SCHDefaultRange(const float & newRange);
	}
}

#endif /* SCHSTDTASKS_H_ */
