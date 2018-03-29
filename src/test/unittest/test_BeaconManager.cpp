/*
 * test_BeaconManager.cpp
 *
 *  Created on: Jan 11, 2018
 *      Author: chuffman93
 */

#include "util/BeaconManager.h"
#include "test/catch.hpp"
#include <math.h>

BeaconManager BM;


TEST_CASE("Build Beacon","[beacon]"){
	struct sysinfo sy;
	int32_t st = getCurrentTime();
	float cpu15 = ((float)sy.loads[2]/6553.6);
	float memory = (100*(259964928.0 - ((float)sy.freeram))/ (259964928.0));

	uint32_t epochNumber = 1234;
	int32_t GPSWeek = 1984;
	float GPSSec = 8854;

	double lat = 40.0150;
	double lon = 105.2705;
	double alt = 1613;
	double r = 6371000 + alt;
	double xPosition = r*cos(lat*M_PI/180)*cos(lon*M_PI/180);
	double yPosition = r*cos(lat*M_PI/180)*sin(lon*M_PI/180);
	double zPosition = r*sin(lat*M_PI/180);
	double xVelocity = 8000 ;
	double yVelocity = 7000 ;
	double zVelocity = 0.114352;
	uint8_t systemMode = 3;
	uint16_t radNumber = 100;
	uint16_t batteryCap = 89;
	int8_t acsMode = 3;


	SECTION("Create beacon struct"){
		BM.createBeaconGPS(st, epochNumber, GPSWeek, GPSSec, xPosition, yPosition,zPosition, xVelocity, yVelocity, zVelocity);
		BM.createBeaconSYS(st,epochNumber,GPSWeek,GPSSec,systemMode,radNumber,batteryCap,acsMode,memory,cpu15);

		REQUIRE(BM.bGPS.satTime == st);
		REQUIRE(BM.bGPS.epochNumber == epochNumber);
		REQUIRE(BM.bGPS.GPSWeek == GPSWeek);
		REQUIRE(BM.bGPS.GPSSec == GPSSec);
		REQUIRE(BM.bGPS.xPosition == xPosition);
		REQUIRE(BM.bGPS.yPosition == yPosition);
		REQUIRE(BM.bGPS.zPosition == zPosition);
		REQUIRE(BM.bGPS.xVelocity == xVelocity);
		REQUIRE(BM.bGPS.yVelocity == yVelocity);
		REQUIRE(BM.bGPS.zVelocity == zVelocity);

		REQUIRE(BM.bSYS.satTime == st);
		REQUIRE(BM.bSYS.epochNumber == epochNumber);
		REQUIRE(BM.bSYS.GPSWeek == GPSWeek);
		REQUIRE(BM.bSYS.GPSSec == GPSSec);
		REQUIRE(BM.bSYS.systemMode == systemMode);
		REQUIRE(BM.bSYS.radNumber == radNumber);
		REQUIRE(BM.bSYS.batteryCap == batteryCap);
		REQUIRE(BM.bSYS.acsMode == acsMode);
		REQUIRE(BM.bSYS.memory == memory);
		REQUIRE(BM.bSYS.cpu15 == cpu15);

	}

	SECTION("Serialize Beacon"){
		REQUIRE(BM.GPS.size() == 0);
		REQUIRE(BM.SYS.size() == 0);
		BM.sendBeacon();
		REQUIRE(BM.GPS.size() == 64);
		REQUIRE(BM.SYS.size() == 30);

	}

	SECTION("Send beacon to Directory for Com"){
		BM.sendBeaconGPS();
		BM.sendBeaconSYS();
		REQUIRE(access(BCN_SYS,F_OK) == 0);
		REQUIRE(access(BCN_GPS,F_OK) == 0);
	}
}

