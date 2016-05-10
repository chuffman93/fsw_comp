/*
 * ErrorStdTasks.h
 *
 *  Created on: Jul 18, 2013
 *      Author: Admin
 */

#ifndef ERRORSTDTASKS_H_
#define ERRORSTDTASKS_H_

#include "core/MultiDataMessage.h"
//#include "boards/board.h"

namespace Phoenix
{
	namespace Servers
	{
		void EPSError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void COMError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void PLDError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void ACSError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void GPSError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void THMError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void SCHError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void CMDError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void CDHError(int opcode, Phoenix::Core::MultiDataMessage * dataMessage);
		void FileHandlerError(int opcode, Phoenix::Core::MultiDataMessage * retMsg);
		
		void THMPowerReset(PowerSubsystemEnum which);

		bool SendEPSReset();
		bool SendCOMReset();
		bool SendPLDReset();
		bool SendACSReset();
		bool SendGPSReset();
	}
}

#endif /* ERRORSTDTASKS_H_ */
