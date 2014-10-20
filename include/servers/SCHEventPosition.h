/*
 *  SCHEventPosition.h
 *
 *  Created on: May 22, 2013
 *      Author: Brian Campuzano
 */

#ifndef SCHEVENTPOSITION_H_
#define SCHEVENTPOSITION_H_

#include "servers/SCHEvent.h"
#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{

		class SCHEventPosition : public SCHEvent
		{
			public:
				SCHEventPosition(ServerLocationIDEnum sub, uint32 op, float range, float x, float y, float z, bool lessThan);

				bool CheckForEvent();

			private:
				float compareRange;
				float posX;
				float posY;
				float posZ;
				bool shorterThanCalc;
		};
	}
}

#endif /* SCHEVENTPOSITION_H_ */
