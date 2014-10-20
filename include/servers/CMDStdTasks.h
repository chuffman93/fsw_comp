/*
 * CMDStdTasks.h
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#ifndef CMDSTDTASKS_H_
#define CMDSTDTASKS_H_

namespace Phoenix
{
	namespace Servers
	{
		//parse command buffer and create packets
		void CMDBufferParse(char * readBuffer, size_t bufferSize);
	}
}

#endif /* CMDSTDTASKS_H_ */
