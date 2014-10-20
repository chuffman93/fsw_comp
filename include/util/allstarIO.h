/*
 * allstario.h
 *
 *  Created on: Mar 26, 2013
 *      Author: Brian Campuzano
 */

#ifndef ALLSTARIO_H_
#define ALLSTARIO_H_

#include "core/StdTypes.h"

#include "utils/compiler.h"
#include "util/cdefs.h"
#include "util/debug.h"
#include "util/kermit.h"

#define KERMIT_DEBUG			0

#if KERMIT_DEBUG && defined(WIN32)
#include <stdio.h>
#define DEBUG_PRINTF(m) printf("%s(%d,0): %s\n", __FILE__, __LINE__, m); fflush(stdout);
#define DEBUG_PRINTF_INT(m,d) printf("%s%d\n", m, d); fflush(stdout);
#define DEBUG_PRINTF_CHAR(c) printf("%d ", c); fflush(stdout);
#else
#define DEBUG_PRINTF(m)
#define DEBUG_PRINTF_INT(m,d)
#define DEBUG_PRINTF_CHAR(c)
#endif // DEBUG

/* Debugging */

#ifdef DEBUG_KERMIT

void dodebug(int fc, UCHAR * label, UCHAR * sval, long nval);

#endif /* DEBUG_KERMIT */

/*  D E V O P E N  --  Open communications device  */
/*

  Call with: string pointer to device name.  This routine should get the
  current device settings and save them so devclose() can restore them.
  It should open the device.  If the device is a serial port, devopen()
  set the speed, stop bits, flow control, etc.
  Returns: 0 on failure, 1 on success.
*/
int devopen();

/*  P K T M O D E  --  Put communications device into or out of packet mode  */
/*
  Call with: 0 to put in normal (cooked) mode, 1 to put in packet (raw) mode.
  For a "dumb i/o device" like an i/o port that does not have a login attached
  to it, this routine can usually be a no-op.
  Returns: 0 on failure, 1 on success.
*/
int pktmode(short on);


/*  D E V S E T T I N G S  */

int devsettings();

/*  D E V R E S T O R E  */

int devrestore(void);

/*  D E V C L O S E  --  Closes the current open communications device  */
/*
  Call with: nothing
  Closes the device and puts it back the way it was found by devopen().
  Returns: 0 on failure, 1 on success.
*/
int devclose(void);

/* I N C H K  --  Check if input waiting */

/*
  Check if input is waiting to be read, needed for sliding windows.  This
  sample version simply looks in the stdin buffer (which is not portable
  even among different Unixes).  If your platform does not provide a way to
  look at the device input buffer without blocking and without actually
  reading from it, make this routine return -1.  On success, returns the
  numbers of characters waiting to be read, i.e. that can be safely read
  without blocking.
*/
int inchk(struct k_data * k);

/*  R E A D P K T  --  Read a Kermit packet from the communications device  */
/*
  Call with:
    k   - Kermit struct pointer
    p   - pointer to read buffer
    len - length of read buffer

  When reading a packet, this function looks for start of Kermit packet
  (k->r_soh), then reads everything between it and the end of the packet
  (k->r_eom) into the indicated buffer.  Returns the number of bytes read, or:
     0   - timeout or other possibly correctable error;
    -1   - fatal error, such as loss of connection, or no buffer to read into.
*/

int readpkt(struct k_data * k, UCHAR *p, int len);

/*  T X _ D A T A  --  Writes n bytes of data to communication device.  */
/*
  Call with:
    k = pointer to Kermit struct.
    p = pointer to data to transmit.
    n = length.
  Returns:
    X_OK on success.
    X_ERROR on failure to write - i/o error.
*/
int tx_data(struct k_data * k, UCHAR *p, int n);

/*  O P E N F I L E  --  Open output file  */
/*
  Call with:
    Pointer to filename.
    Size in bytes.
    Creation date in format yyyymmdd hh:mm:ss, e.g. 19950208 14:00:00
    Mode: 1 = read, 2 = create, 3 = append.
  Returns:
    X_OK on success.
    X_ERROR on failure, including rejection based on name, size, or date.
*/
int openfile(struct k_data * k, UCHAR * s, int mode);

/*  F I L E I N F O  --  Get info about existing file  */
/*
  Call with:
    Pointer to filename
    Pointer to buffer for date-time string
    Length of date-time string buffer (must be at least 18 bytes)
    Pointer to int file type:
       0: Prevailing type is text.
       1: Prevailing type is binary.
    Transfer mode (0 = auto, 1 = manual):
       0: Figure out whether file is text or binary and return type.
       1: (nonzero) Don't try to figure out file type.
  Returns:
    X_ERROR on failure.
    0L or greater on success == file length.
    Date-time string set to yyyymmdd hh:mm:ss modtime of file.
    If date can't be determined, first byte of buffer is set to NUL.
    Type set to 0 (text) or 1 (binary) if mode == 0.
*/

uint32 fileinfo(struct k_data * k, UCHAR * filename, UCHAR * buf, int buflen, short * type, short mode);

/*  R E A D F I L E  --  Read data from a file  */

int readfile(struct k_data * k);


/*  W R I T E F I L E  --  Write data to file  */
/*
  Call with:
    Kermit struct
    String pointer
    Length
  Returns:
    X_OK on success
    X_ERROR on failure, such as i/o error, space used up, etc
*/
int writefile(struct k_data * k, UCHAR * s, int n) ;

/*  C L O S E F I L E  --  Close output file  */
/*
  Mode = 1 for input file, mode = 2 or 3 for output file.

  For output files, the character c is the character (if any) from the Z
  packet data field.  If it is D, it means the file transfer was canceled
  in midstream by the sender, and the file is therefore incomplete.  This
  routine should check for that and decide what to do.  It should be
  harmless to call this routine for a file that that is not open.
*/
int closefile(struct k_data * k, UCHAR c, int mode) ;

#endif /* ALLSTARIO_H_ */
