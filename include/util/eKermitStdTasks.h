/*
 * eKermitStdTasks.h
 *
 *  Created on: Apr 6, 2013
 *      Author: Brian Campuzano
 */

#ifndef EKERMITSTDTASKS_H_
#define EKERMITSTDTASKS_H_

#include "core/StdTypes.h"

// C++ helper function for sending data to COM Digitial
#ifdef __cplusplus
extern "C"
{
#endif

int ekCOMPacketSend(char * buffer, size_t size, LocationIDType server);

bool ekLockFileHandler(void);

void ekUnlockFileHandler(void);

bool ekGetKermitRunState(void);

void ekDebugLed(uint8 led, uint8 state);

#ifdef __cplusplus
}
#endif

#endif /* EKERMITSTDTASKS_H_ */
