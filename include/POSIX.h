#ifndef INC_POSIX_H_
#define INC_POSIX_H_


#include <time.h>
#include <semaphore.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "core/CommandMessage.h"
#include "iostream"
#include <errno.h>

namespace Phoenix
{
	namespace Core
	{
		#define SHARE_TO_THREADS 1
		#define liSUCCESS 0
		#define liFAILURE -1


		/* 10ms */
		//TODO:find all misuses of SEM_WAIT_DELAY and replace with correct things
		//TODO: fix any function that needs to get time....
		//FIXME
		#define MAX_BLOCK_TIME 7
		#define QUEUE_LENGTH 10
		#define MAX_MESSAGE_SIZE sizeof(Phoenix::Core::FSWPacket *)
		#define QUEUE_PERMISSIONS 0644
		#define MSG_PRIO 0
		//TODO: find good replacement for usleep I think

		int getTimeInMilis();

		inline bool vSemaphoreCreateBinary(sem_t * sem){ return (-1 !=sem_init(sem, SHARE_TO_THREADS, 1)); }

		void semWaitDelay(struct timespec *ts, size_t seconds, uint64_t nanoseconds);

		size_t mqCreate(mqd_t * queueHandle, struct mq_attr * queueAttr, char * mqName);

		size_t mq_size(mqd_t queueHandle, struct mq_attr queueAttr);

		bool mq_timed_send(char * queueName, FSWPacket ** inPacket, size_t sec, uint64_t nSec);


		bool mq_timed_receive(char * queueName, FSWPacket ** packetOut, size_t sec, uint64_t nSec);

		bool xSemaphoreTake(sem_t *sem);

		bool xSemaphoreTake(sem_t *sem, size_t sec, uint64_t nSec);

		bool xSemaphoreGive(sem_t *sem);

		void waitUntil(int LastTimeTick, int waitTime);

		//TODO: find a reasonable amount of time to wait to lock the mutex
		bool pthread_mutex_timedlock(pthread_mutex_t * lockHandle, int waitTime);
	}//Core
}//Phoenix

#endif /* POSIX.h */
