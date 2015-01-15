/* Sample system-dependent communications i/o routines for embedded Kermit. */

/*
Author: Frank da Cruz.
Copyright (C) 1995, 2011.
Trustees of Columbia University in the City of New York.
All rights reserved.
See kermit.c for license.
*/

/*
The sample i/o routines for UNIX that provide packet i/o
functions on the console (login) device.
Copy this file, rename it appropriately, and replace the contents
of each routine appropriately for your platform.

Device i/o:

int devopen()    Communications device - open
int pktmode()    Communications device - enter/exit packet mode
int readpkt()    Communications device - read a packet
int tx_data()    Communications device - send data
int devclose()   Communications device - close
int inchk()      Communications device - check if bytes are ready to read

File i/o:

int openfile()   File - open for input or output
uint32 fileinfo() Get input file modtime and size
int readfile()   Input file - read data
int writefile()  Output file - write data
int closefile()  Input or output file - close

Full definitions below, prototypes in kermit.h.

These routines must handle speed setting, parity, flow control, file i/o,
and similar items without the kermit() routine knowing anything about it.
If parity is in effect, these routines must add it to outbound characters
and strip it from inbound characters.
*/

#include "util/allstarIO.h"
#include "util/cdefs.h"
#include "util/debug.h"
#include "util/kermit.h"
#include "utils/compiler.h"
#include "util/CircularBuffer.h"
#include "util/eKermitDefs.h"
#include "util/eKermitStdTasks.h"

#include "HAL/KermitFileFunctions.h"

#include "boards/backplane/dbg_led.h"

/*
In this example, the output file is unbuffered to ensure that every
output byte is commited.  The input file, however, is buffered for speed.
This is just one of many possible implmentation choices, invisible to the
Kermit protocol module.
*/

/* Debugging */

#ifdef DEBUG_KERMIT
//static FILE * dp = (FILE *)0;		/* Debug log */

void dodebug(int fc, UCHAR * label, UCHAR * sval, long nval)
{

}
#endif /* DEBUG_KERMIT */

/*  D E V O P E N  --  Open communications device  */
/*

Call with: string pointer to device name.  This routine should get the
current device settings and save them so devclose() can restore them.
It should open the device.  If the device is a serial port, devopen()
set the speed, stop bits, flow control, etc.
Returns: 0 on failure, 1 on success.
*/
int devopen(char *device)
{
	//COM Hardware already "open"
	return(1);
}

/*  P K T M O D E  --  Put communications device into or out of packet mode  */
/*
Call with: 0 to put in normal (cooked) mode, 1 to put in packet (raw) mode.
For a "dumb i/o device" like an i/o port that does not have a login attached
to it, this routine can usually be a no-op.
Returns: 0 on failure, 1 on success.
*/
int pktmode(short on)
{
	//Already communicating with packets
	return(1);
}


/*  D E V S E T T I N G S  */

int devsettings()
{
	//No settings to save.
	if (!pktmode(ON))
	{
		return(0);
	}
	return(1);
}

/*  D E V R E S T O R E  */

int devrestore(void)
{
	//No settings to restore.
	pktmode(OFF);
	return(1);
}


/*  D E V C L O S E  --  Closes the current open communications device  */
/*
Call with: nothing
Closes the device and puts it back the way it was found by devopen().
Returns: 0 on failure, 1 on success.
*/
int devclose(void)
{
	//devopen() didn't change anything, nothing to restore.
	return(1);
}

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
int inchk(struct k_data * k)
{
	return(k->inputBuf->used);
}

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

int readpkt(struct k_data * k, UCHAR * p, int len)
{
	
	int n;
	short flag;
	UCHAR c;
	int i = 0;
	char * temp;
	
	CircularBuffer * inBuf = k->inputBuf;
	//ekDebugLed(2, LED_TOGGLE);
	
	/*
	Timeout not implemented in this sample.
	It should not be needed.  All non-embedded Kermits that are capable of
	making connections are also capable of timing out, and only one Kermit
	needs to time out.  NOTE: This simple example waits for SOH and then
	reads everything up to the negotiated packet terminator.  A more robust
	version might be driven by the value of the packet-length field.
	*/

	#ifdef F_CTRLC
	short ccn;
	ccn = 0;
	#endif /* F_CTRLC */

	if (!p) {		/* Device not open or no buffer */
		debug(DB_MSG,"readpkt FAIL",0,0);
		return(-1);
	}
	flag = n = 0;                       /* Init local variables */
	
	if(inBuf == NULL)
	{
		DEBUG_PRINTF("	readpkt: buffer is null...");
		return -1;
	}
	
#ifdef WIN32
/*
	switch(k->server)
	{
		case 9:
			DEBUG_PRINTF_INT("	SPACE KERMIT: Read FSWPacket, bytes in buffer: ", inBuf->used)
			DEBUG_PRINTF("	SPACE KERMIT: Read FSWPacket input buffer: ")
			break;
		case 20:
			DEBUG_PRINTF_INT("	GROUND KERMIT: Read FSWPacket, bytes in buffer: ", inBuf->used)
			DEBUG_PRINTF("	GROUND KERMIT: Read FSWPacket input buffer: ")
			break;
		default:
			DEBUG_PRINTF_INT("	KERMIT: Read FSWPacket, bytes in buffer: ", inBuf->used)
	}
	temp = GetCurrentBuffer(inBuf);
	if(temp != NULL)
	{
		//DEBUG_PRINTF(temp);
		
		for(i = 0; i < inBuf->used; i++)
		{
			DEBUG_PRINTF_CHAR(temp[i])
		}
		DEBUG_PRINTF(" ")
		
		free(temp);
	}
	else
	{
		DEBUG_PRINTF("		BUFFER IS NULL")
	}
*/
	
#endif	

	while (0 < inBuf->used) //make sure the input buffer isn't empty
	{
		c = inBuf->buffer[inBuf->tail];
		#ifdef F_CTRLC
		/* In remote mode only: three consecutive ^C's to quit */
		if (k->remote && c == (UCHAR) 3) 
		{
			if (++ccn > 2) 
			{
				return(-1);
			}
			else 
			{
				ccn = 0;
			}
		}
		#endif /* F_CTRLC */

		if (!flag && c != k->r_soh)	/* No start of packet yet */
		{
			inBuf->used--;
			inBuf->tail++;
			if(inBuf->tail == IBUFLEN)
			{
				inBuf->tail = 0;
			}
			continue;                     /* so discard these bytes. */
		}
		if (c == k->r_soh)
		{		/* Start of packet */
			flag = 1;                   /* Remember */
			inBuf->used--;
			inBuf->tail++;
			if(inBuf->tail == IBUFLEN)
			{
				inBuf->tail = 0;
			}
			continue;                   /* But discard. */
		}
		else if (c == k->r_eom	/* FSWPacket terminator */
		|| c == '\012'	/* 1.3: For HyperTerminal */
		)
		{
			inBuf->used--;
			inBuf->tail++;
			if(inBuf->tail == IBUFLEN)
			{
				inBuf->tail = 0;
			}
			DEBUG_PRINTF_INT("		readpkt: bytes read: ", n)
			return(n);
		}
		else
		{                        /* Contents of packet */
			inBuf->used--;
			inBuf->tail++;
			if(inBuf->tail == IBUFLEN)
			{
				inBuf->tail = 0;
			}
			if (n++ > k->r_maxlen)	/* Check length */
			{
				return(0);
			}
			else
			*p++ = c & 0xff;
		}
	}

	//DEBUG_PRINTF("	readpkt: buffer is empty...");
	return(0);
}

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
int tx_data(struct k_data * k, UCHAR *p, int n)
{
	//ekDebugLed(3, LED_TOGGLE);
	if(ekCOMPacketSend((char *) p, (size_t) n, k->server))
	{
		//DEBUG_PRINTF("	tx_data: ekCOMPacketSend failed...");
		return(X_ERROR);
	}
	//DEBUG_PRINTF("	tx_data: ekCOMPacketSend success...");
	return(X_OK);                       /* Success */
}

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
int openfile(struct k_data * k, UCHAR * s, int mode)
{
	int i = 0;
#ifdef WIN32
/*
	switch(k->server)
	{
		case 9:
			DEBUG_PRINTF("	SPACE KERMIT: openfile:")
			DEBUG_PRINTF(s)
			break;
		case 20:
			DEBUG_PRINTF("	GROUND KERMIT: openfile:")
			DEBUG_PRINTF(s)
			break;
		default:
			DEBUG_PRINTF("	KERMIT: openfile...")
	}
	/*
	for(;i < 18; i++)
	{
		DEBUG_PRINTF_CHAR(s[i]);
	}
	DEBUG_PRINTF(" ");
*/
#endif
	
	switch (mode)
	{
		case FILE_READ_MODE:				/* Read */
			DEBUG_PRINTF("		openfile: kermit open file in read mode...");
			if(!KermitOpenFile((char *) s, mode))
			{
				DEBUG_PRINTF("			openfile: failed to open file...");
				debug(DB_LOG,"openfile read error",s,0);
				return(X_ERROR);
			}
			k->eofFlag = FALSE;		//set eof flag to false
			k->s_first   = 1;		/* Set up for getkpt */
			k->zinbuf[0] = '\0';		/* Initialize buffer */
			k->zinptr    = k->zinbuf;	/* Set up buffer pointer */
			k->zincnt    = 0;		/* and count */
			debug(DB_LOG,"openfile read ok",s,0);
			return(X_OK);

		case FILE_WRITE_MODE:				/* Write (create) */
			DEBUG_PRINTF("		openfile: kermit open file in write mode:");
			if(!KermitOpenFile((char *) s, mode))
			{
				DEBUG_PRINTF("			openfile: failed to open file...");
				debug(DB_LOG,"openfile write error",s,0);
				return(X_ERROR);
			}
			debug(DB_LOG,"openfile write ok",s,0);
			return(X_OK);

		case FILE_APPEND_MODE:				/* Append (not used) */
		default:
			DEBUG_PRINTF("			openfile: invalid mode...");
			return(X_ERROR);
	}
}

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
#ifdef F_SCAN
#define SCANBUF 1024
#define SCANSIZ 49152
#endif /* F_SCAN */

uint32 fileinfo(struct k_data * k, UCHAR * filename, UCHAR * buf, int buflen, short * type, short mode)
{
	int fd;
	uint32 length = 0;
	
#ifdef WIN32
/*
	switch(k->server)
	{
		case 9:
			DEBUG_PRINTF("	SPACE KERMIT: fileinfo called...")
			break;
		case 20:
			DEBUG_PRINTF("	GROUND KERMIT: fileinfo called...")
			break;
		default:
			DEBUG_PRINTF("	KERMIT: fileinfo called...")
	}
*/
#endif
	
	if (!buf)
	{
		DEBUG_PRINTF("			fileinfo: buffer is null...");
		return(X_ERROR);
	}
	buf[0] = NULL;
	if (buflen < 18)
	{
		DEBUG_PRINTF("			fileinfo: buffer is too short...");
		return(X_ERROR);
	}
	if(!KermitGetFileInfo((char *) filename, &length)) //open file read only
	{
		DEBUG_PRINTF("			fileinfo: failed to get file info...");
		return(X_ERROR);
	}
	return(length);

}


/*  R E A D F I L E  --  Read data from a file  */

int readfile(struct k_data * k)
{
	Bool EOFFlag = FALSE;
#ifdef WIN32
/*
	switch(k->server)
	{
		case 9:
		DEBUG_PRINTF("	SPACE KERMIT: readfile called...")
		break;
		case 20:
		DEBUG_PRINTF("	GROUND KERMIT: readfile called...")
		break;
		default:
		DEBUG_PRINTF("	KERMIT: readfile called...")
	}
*/
#endif

	if (!k->zinptr)
	{
		#ifdef DEBUG_KERMIT
		//fprintf(dp,"readfile ZINPTR NOT SET\n");
		#endif /* DEBUG_KERMIT */
		DEBUG_PRINTF("			readfile: input buffer is null...");
		return(X_ERROR);
	}
	if (k->zincnt < 1)
	{		/* Nothing in buffer - must refill */
		if (!k->binary)
		{		/* Binary - just read raw buffers */
			//check for eof during last read
			if(k->eofFlag == TRUE)
			{
				DEBUG_PRINTF("			readfile: EOF");
				return(-1);
			}
			k->dummy = 0;
			k->zincnt = KermitReadFile(((char *) k->zinbuf), k->zinlen, &EOFFlag);
			k->eofFlag = EOFFlag;
			debug(DB_LOG,"readfile binary ok zincnt",0,k->zincnt);
		}
		else
		{			/* Text mode needs LF/CRLF handling */
			// ALLSTAR will not use text mode.
			DEBUG_PRINTF("			readfile: text mode error...");
			return(X_ERROR);
		}
		k->zinbuf[k->zincnt] = '\0';	/* Terminate. */
		
		k->zinptr = k->zinbuf;		/* Not EOF - reset pointer */
	}
	DEBUG_PRINTF("			readfile: returning first byte...");
	(k->zincnt)--;			/* Return first byte. */

	debug(DB_LOG,"readfile exit zincnt",0,k->zincnt);
	debug(DB_LOG,"readfile exit zinptr",0,k->zinptr);
	return(*(k->zinptr)++ & 0xff);
}


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
int writefile(struct k_data * k, UCHAR * s, int n)
{
	int rc;
	
#ifdef WIN32
/*
	switch(k->server)
	{
		case 9:
		DEBUG_PRINTF("	SPACE KERMIT: writefile called...")
		break;
		case 20:
		DEBUG_PRINTF("	GROUND KERMIT: writefile called...")
		break;
		default:
		DEBUG_PRINTF("	KERMIT: writefile called...")
	}
*/
#endif

	rc = X_OK;

	debug(DB_LOG,"writefile binary",0,k->binary);

	if (!k->binary)
	{			/* Binary mode, just write it */
		if (!KermitWriteFile((char *) s, n))
		{
			DEBUG_PRINTF("			writefile: failed to write to file...");
			rc = X_ERROR;
		}
	}
	else
	{
		//ALLSTAR does not use text mode!
		DEBUG_PRINTF("			writefile: text mode error...");
		rc = X_ERROR;
	}
	return(rc);
}

/*  C L O S E F I L E  --  Close output file  */
/*
Mode = 1 for input file, mode = 2 or 3 for output file.

For output files, the character c is the character (if any) from the Z
packet data field.  If it is D, it means the file transfer was canceled
in midstream by the sender, and the file is therefore incomplete.  This
routine should check for that and decide what to do.  It should be
harmless to call this routine for a file that that is not open.
*/
int closefile(struct k_data * k, UCHAR c, int mode)
{
	int rc = X_OK;			/* Return code */
	
#ifdef WIN32
/*
	switch(k->server)
	{
		case 9:
		DEBUG_PRINTF("	SPACE KERMIT: closefile called...")
		break;
		case 20:
		DEBUG_PRINTF("	GROUND KERMIT: closefile called...")
		break;
		default:
		DEBUG_PRINTF("	KERMIT: closefile called...")
	}
*/
#endif

	switch (mode)
	{
	case FILE_READ_MODE:				/* Closing input file */
		DEBUG_PRINTF("			close file: closing input file...");
		debug(DB_LOG,"closefile (input)",k->filename,0);
		if (!KermitCloseFile(mode)) //read mode
		{
			DEBUG_PRINTF("			closefile: failed to close input file...");
			rc = X_ERROR;
		}
		break;
	case FILE_WRITE_MODE:				/* Closing output file */
		DEBUG_PRINTF("			closefile: closing ouput file...");
		debug(DB_LOG,"closefile (output) name",k->filename,0);
		debug(DB_LOG,"closefile (output) keep",0,k->ikeep);
		if (!KermitCloseFile(mode))
		{		/* Try to close */
			DEBUG_PRINTF("			closefile: failed to close output file...");
			rc = X_ERROR;
		}
		if ((k->ikeep == 0) && (c == 'D')) /* Don't keep incomplete files */
		{	/* This file was incomplete */
			if (k->filename)
			{
				DEBUG_PRINTF("			closefile: delete partial ouput file...");
				debug(DB_LOG,"deleting incomplete",k->filename,0);
				if(!KermitDeleteFile(((char *) k->filename)))	/* Delete it. */
				{
					DEBUG_PRINTF("			closefile: failed to delete file...");
					rc = X_ERROR;
				}
			}
		}
	break;
	case FILE_APPEND_MODE: //No append mode
	default:
		DEBUG_PRINTF("			closefile: invalid mode...");
		rc = X_ERROR;
	}
	return(rc);
}