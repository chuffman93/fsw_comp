#include "POSIX.h"

using namespace std;

namespace Phoenix
{
	namespace Core
	{
		int getTimeInMilis()
		{
			struct timeval now;
			gettimeofday(&now, NULL);
			return ((now.tv_sec) * 1000 + now.tv_usec / 1000);

		}

		void semWaitDelay(struct timespec *ts, size_t seconds, uint64_t nanoseconds)//inline this
		{
			clock_gettime(CLOCK_REALTIME, ts);
			ts->tv_sec += seconds;
			ts->tv_nsec += nanoseconds;
		}

		size_t mqCreate(mqd_t * queueHandle, struct mq_attr * queueAttr, char * mqName)
		{
			mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
			queueAttr->mq_flags = 0;
			queueAttr->mq_maxmsg = QUEUE_LENGTH;
			queueAttr->mq_msgsize = MAX_MESSAGE_SIZE;
			//printf("Opening Queue with %lu bytes max message size\n", MAX_MESSAGE_SIZE);
			queueAttr->mq_curmsgs = 0;

			if(-1 == ((*queueHandle)= mq_open(mqName, O_RDWR | O_NONBLOCK | O_CREAT, mode, queueAttr)))
			{
				printf("mq_open failed with ERRORNO = %s\n", strerror(errno));
				return liFAILURE;
			}
			//printf("Queue %s Init Successful\n", mqName);
			return liSUCCESS;
		}

		size_t mq_size(mqd_t queueHandle, struct mq_attr queueAttr)
		{
			mq_getattr(queueHandle,&queueAttr);
			return (size_t)queueAttr.mq_curmsgs;
		}

		bool mq_timed_send(char * queueName, FSWPacket ** inPacket, size_t sec, uint64_t nSec)
		{
			struct timespec ts;
			semWaitDelay(&ts, sec, nSec);

			mqd_t queueHandle = mq_open(queueName, O_WRONLY);

//			struct mq_attr queueAttr;
//			mq_getattr(queueHandle, &queueAttr);
			if(-1 == mq_timedsend(queueHandle, (char *) inPacket, sizeof(*inPacket), MSG_PRIO, &ts))
			{
				printf("mq_timed_Send failed with ERRORNO = %s\n", strerror(errno));
				mq_close(queueHandle);
				return false;
			}
			mq_close(queueHandle);
			return true;
		}

		bool mq_timed_receive(char * queueName, FSWPacket ** packetOut, size_t sec, uint64_t nSec)
		{
			struct timespec ts;
			semWaitDelay(&ts, sec, nSec);

			ssize_t msg_len;
			mqd_t queueHandle = mq_open(queueName, O_RDONLY);

			struct mq_attr queueAttr;
			mq_getattr(queueHandle, &queueAttr);

			if((msg_len = mq_timedreceive(queueHandle,(char *) packetOut, (queueAttr.mq_msgsize), MSG_PRIO, &ts)) < 0)
			{

				//printf("mq_timed_recieve failed with ERRORNO = %s\n", strerror(errno));
				mq_close(queueHandle);
				return false;
			}
			mq_close(queueHandle);
			//printf("Received %ld bytes from the queue\n", msg_len);
			return true;
		}

		bool xSemaphoreTake(sem_t *sem)
		{
			return (liSUCCESS == sem_wait(sem));
		}

		bool xSemaphoreTake(sem_t *sem, size_t sec, uint64_t nSec)
		{
			struct timespec ts;
			semWaitDelay(&ts, sec, nSec);
			if(liSUCCESS != sem_timedwait(sem, &ts))
			{
				printf("ERRORNO = %s\n", strerror(errno));
				return false;
			}
			return true;
		}

		bool xSemaphoreGive(sem_t *sem)
		{
			int ret = sem_post(sem);
			if(ret == 0)
			{
				return true;
			}
			else if((ret == -1) && EOVERFLOW == errno)
			{
				return true;
			}
			return false;
		}


		void waitUntil(int LastTimeTick, int waitTimeMS)
		{


			clock_t this_time = getTimeInMilis();
			clock_t last_time = this_time;
			double time_counter = (this_time - LastTimeTick);

			while(time_counter < (waitTimeMS))
			{
				this_time = getTimeInMilis();

				time_counter += (this_time - last_time);

				last_time =  this_time;
			}

		}

		//TODO: find a reasonable amount of time to wait to lock the mutex
		bool pthread_mutex_timedlock(pthread_mutex_t * lockHandle, int waitTime)
		{
			clock_t this_time = getTimeInMilis();
			clock_t last_time = this_time;
			double time_counter = 0;

			while(time_counter < (waitTime * 1000))
			{
				if(0 == pthread_mutex_trylock(lockHandle))
				{
					return true;
				}
				this_time = getTimeInMilis();

				time_counter += (this_time - last_time);

				last_time =  this_time;

			}
			return false;
		}
		}// Core
}//Phoenix
