/* test0003.cpp
 * Created on: Jun 23, 2014
 *    Author: fsw
 * \brief General test 3
 * 
 *  Tests that xSemaphoreTake waits SEM_WAIT_DELAY seconds before returning false
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
using namespace AllStar;
using namespace Core;
using namespace std;



TEST(BasicTest, SEMWAITDELAY)
{
	sem_t testSem3;
	vSemaphoreCreateBinary(&testSem3);

	xSemaphoreTake(&testSem3, 0, 0);
	int start_time = getTimeInMilis();
	xSemaphoreTake(&testSem3, 1, 0);
	start_time = getTimeInMilis() - start_time;
	xSemaphoreGive(&testSem3);


	sem_destroy(&testSem3);
	ASSERT_TRUE(start_time >= 1000);
	ASSERT_TRUE(start_time <= 1100);
	
}
