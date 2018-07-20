/*
 * test_GPS.cpp
 *
 *  Created on: Jan 2, 2018
 *      Author: cyborg9
 */

#include "test/catch.hpp"
#include "test/testmacros.h"
#include "subsystem/GPS.h"
#include <unistd.h>
#include <fstream>
#include "util/TimeKeeper.h"
#include "test/swintegration/MockSubPower.h"
#include "test/swintegration/MockNMEA.h"


UARTManager dummyu("");
GPIOManager dummyg("");


std::string teststr = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141600.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,6800000.00,0.00,0.00,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,0.00,500.00,7600.00,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
//std::string teststr = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141600.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-424512.363,-6786732.88,11179.885,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,499.107,-29.901,7600.00,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
// ^^^^ string for shortened calc sleep

TEST_CASE("Test GPS fetchNewGPS", "[.][subsystem][gps]"){
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	initializeTime();
	nm.setString(teststr);
	gps.fetchNewGPS();
	GPSPositionTime pt = gps.getBestXYZI();
	REQUIRE(fabs(pt.posX - 6878.14) < 0.01);
	REQUIRE(fabs(pt.posY - 0) < 0.01);
	REQUIRE(fabs(pt.posZ - 0) < 0.01);
	REQUIRE(fabs(pt.velX - 0) < 0.01);
	REQUIRE(fabs(pt.velY - 0) < 0.01);
	REQUIRE(fabs(pt.velZ - 7.612607) < 0.01);
	REQUIRE(pt.GPSWeek == 1981);
	REQUIRE(pt.GPSSec  == 140418);
}

TEST_CASE("Test that it doesn't set when the string is invalid", "[subsystem][gps][op]"){
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	nm.setString(teststr);
	initializeTime();
	gps.fetchNewGPS();
	GPSPositionTime old = gps.getBestXYZI();
	nm.setString("B" + teststr); //Give an invalid char at the front
	gps.fetchNewGPS();

	GPSPositionTime pt = gps.getBestXYZI();

	REQUIRE(fabs(old.posX - pt.posX) < 0.01);
	REQUIRE(fabs(old.posY - pt.posY) < 0.01);
	REQUIRE(fabs(old.posZ - pt.posZ) < 0.01);
	REQUIRE(fabs(old.velX - pt.velX) < 0.01);
	REQUIRE(fabs(old.velY - pt.velY) < 0.01);
	REQUIRE(fabs(old.velZ - pt.velZ) < 0.01);
	REQUIRE(old.GPSWeek == pt.GPSWeek);
	REQUIRE(old.GPSSec == pt.GPSSec);
}


TEST_CASE("Test GPS Orbital Propagator", "[subsystem][gps][op]"){
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	// need to run propagater over a some time
	initializeTime();
	nm.setString(teststr);
	gps.fetchNewGPS();
	GPSPositionTime pt;
	uint32_t timeFSW = getCurrentTime();
	pt = gps.getBestXYZI();
	// file to visually test
	std::ofstream o("GPSData.csv", std::ofstream::out);
	o << timeFSW << "," << pt.posX << "," << pt.posY << "," << pt.posZ << "\n";
	for(int i = 0; i <= 240; i++){
		uint32_t tempT = 60;
		timeFSW = spoofTime(tempT);
		pt = gps.getBestXYZI();
		o << timeFSW << "," << pt.posX << "," << pt.posY << "," << pt.posZ << "\n";
	}
	o.close();
}

TEST_CASE("Test GPS Config", "[.][subsystem][gps][cf]"){
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	gps.initialize();
	uint16_t to = 900;
	uint16_t ti = 7200;
	REQUIRE(gps.timeout == to);
	REQUIRE(gps.timein == ti);

}

TEST_CASE("GPS time calculation until in range","[.][subsystem][gps][ttr]"){
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	initializeTime();
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_INFO);
	Logger::registerFilter(LogTag("Name", "GPS"), LEVEL_DEBUG);
	gps.initialize();
	gps.fetchNewGPS();
	for(int i = 0; i < 15; i++){
		gps.getBestXYZI();
		sleep(1);
	}
	uint64_t tts = gps.calcSleepTime(gps.getBestXYZI());
	cout << tts << endl;
}
