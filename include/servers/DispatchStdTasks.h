/*
 * DispatchStdTasks.h
 *
 * Created: 7/26/2013 11:22:36 AM
 *  Author: Admin
 */ 


#ifndef DISPATCHSTDTASKS_H_
#define DISPATCHSTDTASKS_H_

#include <list>

#include "core/ReturnMessage.h"
#include "core/MultiDataMessage.h"
#include "core/StdTypes.h"
#include "core/VariableTypeEnum.h"

#define DIS_DEBUG			0

#if DIS_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#endif // DEBUG

namespace Phoenix
{
	namespace Servers
	{
		// Send packet to the dispatcher
		Phoenix::Core::FSWPacket * DispatchPacket(Phoenix::Core::FSWPacket * packet);

		// Process a response packet
		void PacketProcess(LocationIDType id, Phoenix::Core::FSWPacket * retPacket);

		// Functions for extracting parameters into buffers
		uint32 	GetUInt(uint8 * buffer);
		uint16 	GetUInt16(uint8 * buffer);
		int32 	GetInt(uint8 * buffer);
		bool 	GetBool(uint8 * buffer);
		float 	GetFloat(uint8 * buffer);
		double 	GetDouble(uint8 * buffer);

		// Functions for adding parameters to buffers
		void AddUInt32(uint8 * buffer, uint32 data);
		void AddFloat(uint8 * buffer, float data);
	}
}


#endif /* DISPATCHSTDTASKS_H_ */
