/* testfilehandler0001.cpp
 * Created on: Jun 18, 2014
 *    Author: pedro
 *  \brief Test 1 of the File Handler.
 *
 *
 */
#include <list>
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "HAL/RTC.h"

using namespace Phoenix::HAL;
using namespace std;

TEST(rtc, SETandGET)
{
	{
		uint32 sec;
		uint16 nsec;
		ASSERT_TRUE(Phoenix::HAL::RTCGetTime(&sec, &nsec));
		cout <<"[----------] " << sec << "." << nsec << " seconds" << endl;
		ASSERT_TRUE(Phoenix::HAL::RTCSetTime(0));
		ASSERT_TRUE(Phoenix::HAL::RTCGetTime(&sec, &nsec));
		cout <<"[----------] " << sec << "." << nsec << " seconds" << endl;
		ASSERT_TRUE(Phoenix::HAL::RTCGetTime(&sec, &nsec));
		cout <<"[----------] " << sec << "." << nsec << " seconds" << endl;
		ASSERT_TRUE(Phoenix::HAL::RTCGetTime(&sec, &nsec));
		cout <<"[----------] " << sec << "." << nsec << " seconds" << endl;
		ASSERT_TRUE(Phoenix::HAL::RTCGetTime(&sec, &nsec));
		cout <<"[----------] " << sec << "." << nsec << " seconds" << endl;
		ASSERT_TRUE(Phoenix::HAL::RTCGetTime(&sec, &nsec));
		cout <<"[----------] " << sec << "." << nsec << " seconds" << endl;
	}
}
