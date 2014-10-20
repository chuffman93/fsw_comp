/*
 * ScheduledTaskPayloadTime.h
 *
 *  Created on: May 22, 2013
 *      Author: Brian Campuzano
 */

#ifndef SCHEVENTTIME_H_
#define SCHEVENTTIME_H_

#include "servers/SCHEvent.h"
#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{

		class SCHEventTime : public SCHEvent
		{
			public:
				SCHEventTime(ServerLocationIDEnum sub, uint32 op, int week, float sec);

				bool CheckForEvent();

			private:
				int compareWeek;
				float compareSec;
		};
	}
}

#endif /* SCHEVENTTIME_H_ */
