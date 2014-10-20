/*
 * KermitFileFunctions.h
 *
 * Created: 8/26/2013 10:28:47 AM
 *  Author: Admin
 */ 


#ifndef KERMITFILEFUNCTIONS_H_
#define KERMITFILEFUNCTIONS_H_

#include "utils/compiler.h"

#include "core/StdTypes.h"

/**************************WARNING****************************
 * 															 *
 * 	The following functions assume that the SD card has		 *
 *  been locked outside of Kermit. Using these functions	 *
 *  without locking the SD card may produce corrupted files  *
 *  														 *
 **************************WARNING***************************/
int32 KermitGetFileInfo(char * file, char * buffer);
Bool KermitInternalOpenFile(char * file, int mode);
Bool KermitOpenFile(char * file, int mode);
int32 KermitReadFile(char * buffer, uint32 sizeToRead, Bool * endOfFileFlag);
Bool KermitWriteFile(char * buffer , int numBytes);
Bool KermitInternalCloseFile(int mode);
Bool KermitCloseFile(int mode);
Bool KermitDeleteFile(char * fileName);
Bool KermitLastModTime(char * file, char * buffer);
uint32 KermitFileLength(void);

#endif /* KERMITFILEFUNCTIONS_H_ */