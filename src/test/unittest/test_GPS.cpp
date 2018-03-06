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


UARTManager dummyu("");
GPIOManager dummyg("");


std::string teststr = "BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140418.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,5197700.0,4504720.0,11860.5,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,318.377,-387.397,7612.6,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
//std::string teststr = "BESTXYZA,COM1,0,55.0,FINESTEERING,1957,240960.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,4792805.471,-3190283.834,-3762966.202,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-3699.37,1958.58,-6372.311,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";


class MockNMEA: public NMEAInterface{
public:
	MockNMEA():NMEAInterface(dummyu){}
	std::string getString(){
		return teststr;
	}

	void sendCommand(std::string str){

	}
	std::string teststr;
};

class MockPower: public SubPowerInterface{
public:
	MockPower(): SubPowerInterface(dummyg, -1, -1, -1, ""){}

	void configDelay(size_t resetdelay, size_t startupdelay){}

	void powerOff(){}
	void powerOn(){}
	void reset(){}
	bool faultOccurred(){return false;}
};


TEST_CASE("Test GPS fetchNewGPS", "[.][subsystem][gps]"){
	MockNMEA nm;
	MockPower pow;
	GPS gps(nm, pow);
	initializeTime();
	nm.teststr = teststr;
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
	MockPower pow;
	GPS gps(nm, pow);
	nm.teststr = teststr;
	initializeTime();
	gps.fetchNewGPS();
	GPSPositionTime old = gps.getBestXYZI();
	nm.teststr = "B" + teststr; //Give an invalid char at the front
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
	MockPower pow;
	GPS gps(nm, pow);
	// need to run propagater over a some time
	initializeTime();
	nm.teststr = teststr;
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

