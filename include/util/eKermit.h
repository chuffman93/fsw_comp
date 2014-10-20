/*
 * eKermitThread.h
 *
 *  Created on: Mar 26, 2013
 *      Author: Brian Campuzano
 */

#ifndef EKERMIT_H_
#define EKERMIT_H_

#include "core/StdTypes.h"

int RunKermit(int mode, int numFiles, UCHAR ** fileList, UCHAR * fileInputBuffer, UCHAR * fileOutputBuffer, 
	CircularBuffer * inputBuffer, LocationIDType server);

#endif /* EKERMIT_H_ */
