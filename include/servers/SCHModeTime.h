/*
 * ScheduledTaskTime.h
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#ifndef SCHMODETIME_H_
#define SCHMODETIME_H_

#include "servers/SCHMode.h"

#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{

		class SCHModeTime : public SCHMode
		{
			public:
				SCHModeTime(SystemModeEnum mode, int week, float sec);

				bool CheckForSwitch();

			private:
				int compareWeek;
				float compareSec;
		};
	}
}

#endif /* SCHMODETIME_H_ */
