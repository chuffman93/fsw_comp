#include "core/StdTypes.h"
#include <signal.h>

namespace Phoenix
{
    namespace HAL
    {

    	bool initSigHandling(void * handlerFunc,int signum, struct sigaction *act)
    	{
        	memset (act, '\0', sizeof(act));
        	act->sa_sigaction = handlerFunc;
        	act->sa_flags = SA_SIGINFO;
        	if (sigaction(signum, act, NULL) < 0)
        	{
        		return false;
        	}
        	else
        	{
        		return true;
        	}
    	}

    }
}
