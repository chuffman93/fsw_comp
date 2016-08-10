/*
 * 	COMStdTasks.h
 * 	Written by: Brian Campuzano
 * 	   Created: 10/20/2012
 */

#ifndef COMSTDTASKS_H_
#define COMSTDTASKS_H_

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/DataMessage.h"
#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/VariableTypeData.h"

#include "servers/SubsystemServer.h"
#include "servers/COMServer.h"

namespace AllStar
{
	namespace Servers
	{
		// \brief Collects health and status from COM.
		AllStar::Core::FSWPacket * COMHealthStatus(void);

		// \brief Sends a beacon to COM.
		AllStar::Core::FSWPacket * COMBeacon(void);

		// \brief Sends a data packet to COM.
		AllStar::Core::FSWPacket * COMDataSend(const std::vector<uint8> & data, LocationIDType server);

		// \brief Resets the COM subsystem.
		AllStar::Core::FSWPacket * COMReset(void);

		// \brief Adds data to the input buffer, and parses it if the COM server is the waiting for command state.
		AllStar::Core::FSWPacket * COMDataReceive(std::vector<uint8> & data, LocationIDType server);

		// \brief Transitions COM Server state from beacon to waiting for command.
		AllStar::Core::FSWPacket * COMLogin(LocationIDType server);

		// \brief Transition COM Server to beacon, kills eKermit if it is running.
		AllStar::Core::FSWPacket * COMLogout(LocationIDType server, MessageCodeType opcode);

		// \brief Sends a packet to the ground station (Packetception)
		void DispatchGndPacket(MessageTypeEnum type, MessageCodeType opCode, std::list<AllStar::Core::VariableTypeData*> parameters);

		// \brief Tries to construct a packet from the input buffer.
		// 		Executes if it is a valid packet
		bool ParseGndCommand(LocationIDType server);

		// \brief Task to start eKermit to send files to the ground.
		void KermitSendTask(void * params);

		// \brief Task to start eKermit to receive files from the ground.
		void KermitReceiveTask(void * params);
	}
}

#endif /* COMSTDTASKS_H_ */
