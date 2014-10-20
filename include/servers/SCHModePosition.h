/*
 * ScheduledTaskPosition.h
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#ifndef SCHMODEPOSTION_H_
#define SCHMODEPOSTION_H_

#include "servers/SCHMode.h"

#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{

		class SCHModePosition : public SCHMode
		{
			public:
				SCHModePosition(SystemModeEnum mode, float range, float x, float y, float z, bool lessThan);

				bool CheckForSwitch();
				
			private:
				float compareRange;
				float posX;
				float posY;
				float posZ;
				bool shorterThanCalc;
		};
	}
}

#endif /* SCHMODEPOSTION_H_ */
