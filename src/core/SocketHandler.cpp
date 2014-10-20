/*
 * SocksUtil.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: fsw
 */
//#include "core/SocketHandler.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/WatchdogManager.h"

#include "POSIX.h"

#include <iostream>

namespace phoenix
{
	namespace core
	{
		int socksListen(const char * name)
		{
			int                 fd, len, err, rval;
			struct sockaddr_un  un;

			/* create a UNIX domain stream socket */
			if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
			{
				return(-1);
			}
			unlink(name);   /* in case it already exists */

			/* fill in socket address structure */
			memset(&un, 0, sizeof(un));
			un.sun_family = AF_UNIX;
			strcpy(un.sun_path, name);
			len = offsetof(struct sockaddr_un, sun_path) + strlen(name);

			/* bind the name to the descriptor */
			if (bind(fd, (struct sockaddr *)&un, len) < 0) {
				rval = -2;
				goto errout;
			}
			return(fd);

		errout:
			err = errno;
			close(fd);
			errno = err;
			return(rval);
		}

		int socksConnect(const char * server_name, const char * client_name)
		{
			int                fd, len, err, rval;
			struct sockaddr_un un;

			/* create a UNIX domain stream socket */
			if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
				return(-1);

			/* fill socket address structure with our address */
			memset(&un, 0, sizeof(un));
			un.sun_family = AF_UNIX;
			strcpy(un.sun_path, client_name);
			len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

			unlink(un.sun_path);        /* in case it already exists */
			if (bind(fd, (struct sockaddr *)&un, len) < 0) {
				rval = -2;
				goto errout;
			}
			if (chmod(un.sun_path, CLI_PERM) < 0) {
				rval = -3;
				goto errout;
			}
			/* fill socket address structure with server's address */
			memset(&un, 0, sizeof(un));
			un.sun_family = AF_UNIX;
			strcpy(un.sun_path, server_name);
			len = offsetof(struct sockaddr_un, sun_path) + strlen(server_name);
			if (connect(fd, (struct sockaddr *)&un, len) < 0) {
				rval = -4;
				goto errout;
			}
			return(fd);

		errout:
			err = errno;
			close(fd);
			errno = err;
			return(rval);
		}
	}// core
} // phoenix
