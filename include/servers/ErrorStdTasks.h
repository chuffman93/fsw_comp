/*
 * ErrorStdTasks.h
 *
 *  Created on: Jul 18, 2013
 *      Author: Admin
 */

#ifndef ERRORSTDTASKS_H_
#define ERRORSTDTASKS_H_

namespace AllStar{
namespace Servers{

void EPSError(int opcode, AllStar::Core::ACPPacket * packet);
void COMError(int opcode, AllStar::Core::ACPPacket * packet);
void PLDError(int opcode, AllStar::Core::ACPPacket * packet);
void ACSError(int opcode, AllStar::Core::ACPPacket * packet);
void GPSError(int opcode, AllStar::Core::ACPPacket * packet);
void SCHError(int opcode, AllStar::Core::ACPPacket * packet);
void CMDError(int opcode, AllStar::Core::ACPPacket * packet);
void CDHError(int opcode, AllStar::Core::ACPPacket * packet);
void FileHandlerError(int opcode, AllStar::Core::ACPPacket * packet);

void THMPowerReset(PowerSubsystemEnum which);

bool SendEPSReset();
bool SendCOMReset();
bool SendPLDReset();
bool SendACSReset();
bool SendGPSReset();
}
}

#endif /* ERRORSTDTASKS_H_ */
