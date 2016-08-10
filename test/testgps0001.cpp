#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include <sys/time.h>
#include "HAL/GPS.h"

using namespace std;
using namespace rel_ops;
using namespace AllStar::HAL;


TEST(TestGPS, TestUnixConversion) {
	//Fri, 13 Feb 2009 23:31:30 GMT == 1234567890 UnixTime == 918603105 seconds GPStime
	struct timeval unixTime;

	//GPS week and second representation 13 Feb 23:31:30
	unsigned long GPSWeek = 1518;
	float GPSsec = 432000 + (60 * 60 *23) + (60 *31) + 30;
	GPStoUnixTime(&unixTime, GPSWeek, GPSsec);

	ASSERT_EQ(1234567890,unixTime.tv_sec);
}

TEST(TestGPS, SettingValidTime){
	struct timeval unixTime,sysTime;
	unixTime.tv_sec = 1234567890;
	unixTime.tv_usec = 0;

	ASSERT_TRUE(SetSystemTime(&unixTime));

	gettimeofday(&sysTime, NULL);

	//ensure system time was set properly
	EXPECT_EQ(sysTime.tv_sec,unixTime.tv_sec);
}

TEST(TestGPS, GPSReceiveFullTest){
	//Have char buffer, parse it properly, then set time and date
}

