#include "gtest/gtest.h"
#include "POSIX.h"
#include <iostream>
using namespace std;
using namespace AllStar;
using namespace Core;
sem_t xSemaphore;


static void * taskTest(void * parameters)
{
	static int i = 0;
	while (1)
	{
		xSemaphoreTake(&xSemaphore, 0, 1000000);
		xSemaphoreGive(&xSemaphore);
		usleep(1000);

		if (i++ == 10)
		{
			cout << "Test Passed" << endl;
			pthread_exit(0);
		}
	}
}

static void * taskTest2(void * parameters)
{

	while (1)
	{
		xSemaphoreTake(&xSemaphore, 0, 1000000);
		xSemaphoreGive(&xSemaphore);
		usleep(1000);
	}
}

TEST(BasicTest, CorrectSemaphores){
	cout << "Test0001" << endl;
	ASSERT_NE(vSemaphoreCreateBinary(&xSemaphore ), false);



	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread1, NULL);

	sem_destroy(&xSemaphore);
}
