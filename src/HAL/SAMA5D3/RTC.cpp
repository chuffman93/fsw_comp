/*
 */

#include "core/StdTypes.h"
#include "HAL/RTC.h"
#include "POSIX.h"
#include <time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <unistd.h>

extern "C"
{
}

// used to prevent multiple threads accessing aes hardware simultaneously
static pthread_mutex_t Mutex;

namespace AllStar
{
    namespace HAL
    {
        void RTCOSInit(void)
        {
        	pthread_mutex_init(&Mutex, NULL);
        }

        bool RTCInit(void)
        {
        	if(pthread_mutex_lock(&Mutex) != 0)
        		return false;
        	pthread_mutex_unlock(&Mutex);
            return true;
        }

        bool RTCGetTime(uint32 *seconds, uint16 *count)
		{
        	if(pthread_mutex_lock(&Mutex) != 0)
        		return false;
        	//struct timespec ts;
        	//if(0 != clock_gettime(0,&ts)){
        	struct rtc_time rtcTime;
        	int fd;
        	fd = open ("/dev/rtc0", O_RDONLY);
        	if (fd ==  -1) {
        		perror("/dev/rtc0");
				pthread_mutex_unlock(&Mutex);
				return false;
           	}
        	if(0 != ioctl(fd, RTC_RD_TIME, &rtcTime)){
        		close(fd);
				pthread_mutex_unlock(&Mutex);
				return false;
			}
//			if (count)
//				*count = ts.tv_nsec;
			if (seconds)
				*seconds = ((rtcTime.tm_hour*60)+rtcTime.tm_min)*60 + rtcTime.tm_sec;
			close(fd);
			pthread_mutex_unlock(&Mutex);
			return true;
		}

        bool RTCSetTime(uint32 seconds)
		{
        	if(pthread_mutex_lock(&Mutex) != 0)
        		return false;
//        	struct timespec ts;
//			ts.tv_sec = seconds;
//			ts.tv_nsec = 1;
			//if(0 != clock_settime(0,&ts)){
			struct rtc_time rtcTime;
			int fd = open ("/dev/rtc0", O_WRONLY);
			if (fd ==  -1) {
				perror("/dev/rtc0");
				pthread_mutex_unlock(&Mutex);
				return false;
			}
			if(0 != ioctl(fd, RTC_RD_TIME, &rtcTime)){
				close(fd);
				pthread_mutex_unlock(&Mutex);
				return false;
			}
			rtcTime.tm_hour = seconds/3600;
			rtcTime.tm_min = (seconds % 3600) / 60;
			rtcTime.tm_sec = ((seconds % 3600) % 60);
			if(0 != ioctl(fd, RTC_SET_TIME, &rtcTime)){
				perror(strerror(errno));
				close(fd);
				pthread_mutex_unlock(&Mutex);
				return false;
			}
			close(fd);
			pthread_mutex_unlock(&Mutex);
			return true;
		}
    }
}
