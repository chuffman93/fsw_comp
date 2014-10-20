/*
 * eKermitDefs.h
 *
 *  Created on: Mar 27, 2013
 *      Author: Brian Campuzano
 */

#ifndef EKERMITDEFS_H_
#define EKERMITDEFS_H_

#include "core/StdTypes.h"

#define MINSIZE //Make the kermit as small as possible.

#define NODEBUG

#define IBUFLEN  1024			/* File input buffer size */
#define OBUFLEN  1024           /* File output buffer size */

#define FN_MAX   32           /* Maximum filename length */

#define FILE_READ_MODE	1
#define FILE_WRITE_MODE	2
#define FILE_APPEND_MODE 3	//Not used on ALLSTAR

#define P_PKTLEN ((MAX_FRAME_LENGTH*MAX_NUMBER_FRAMES)-PACKET_OVERHEAD_SIZE)		/* Kermit max packet length */
#define P_WSLOTS 16			/* Kermit windows */

#define P_S_TIMO   1200                 /* Timeout to tell other Kermit  *///was 40
#define P_R_TIMO   1200                 /* Default timeout for me to use *///was 5
#define P_RETRY    600					/* Per-packet retramsit limit    *///was 10

#define KERMIT_CHECK_FOR_DATA_LOOP 100  //loop running at 10ms*100 = 1second

#endif /* EKERMITDEFS_H_ */
