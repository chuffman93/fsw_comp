/*
 * test_GPS.cpp
 *
 *  Created on: Jan 2, 2018
 *      Author: cyborg9
 */

#include "test/catch.hpp"
#include "test/testmacros.h"
#include "subsystem/GPS.h"

UARTManager dummyu("");
GPIOManager dummyg("");
std::string teststr = "BESTXYZA,COM1,0,55.0,FINESTEERING,1957,240960.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,4792805.471,-3190283.834,-3762966.202,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-3699.37,1958.58,-6372.311,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";


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

TEST_CASE("Test GPS fetchNewGPS", "[subsystem][gps]"){
	MockNMEA nm;
	MockPower pow;
	GPS gps(nm, pow);

	SECTION("Test nominal"){
		nm.teststr = teststr;
		gps.fetchNewGPS();
		GPSPositionTime pt = gps.getBestXYZ();

		cout << "POS { " << pt.posX << "," << pt.posY << "," << pt.posZ << " }" << endl;
		cout << "VEL { " << pt.velX << "," << pt.velY << "," << pt.velZ << " }" << endl;


		REQUIRE(fabs(pt.posX - 4792.805471) < 0.15);
		REQUIRE(fabs(pt.posY - -3190.283834) < 0.15);
		REQUIRE(fabs(pt.posZ - -3762.966202) < 0.15);
		REQUIRE(fabs(pt.velX - -3.69937) < 0.15);
		REQUIRE(fabs(pt.velY - 1.95858) < 0.15);
		REQUIRE(fabs(pt.velZ - -6.372311) < 0.15);
		REQUIRE(pt.GPSWeek == 1957);
		REQUIRE(pt.GPSSec  == 240960);
	}


	SECTION("Test that it doesn't set when the string is invalid"){
		GPSPositionTime old = gps.getBestXYZ();
		nm.teststr = "B" + teststr; //Give an invalid char at the front
		gps.fetchNewGPS();

		GPSPositionTime pt = gps.getBestXYZ();

		REQUIRE(fabs(old.posX - pt.posX) < 0.01);
		REQUIRE(fabs(old.posY - pt.posY) < 0.01);
		REQUIRE(fabs(old.posZ - pt.posZ) < 0.01);
		REQUIRE(fabs(old.velX - pt.velX) < 0.01);
		REQUIRE(fabs(old.velY - pt.velY) < 0.01);
		REQUIRE(fabs(old.velZ - pt.velZ) < 0.01);
		REQUIRE(old.GPSWeek == pt.GPSWeek);
		REQUIRE(old.GPSSec == pt.GPSSec);
	}

	/* Commented out since FSW doesn't actually check this stuff rn
	SECTION("Test that it doesn't set when the crc is invalid"){
		GPSPositionTime old = gps.getBestXYZ();
		nm.teststr = teststr.substr(0, teststr.length() - 2) + "ao"; //Give an invalid char at the front
		gps.fetchNewGPS();

		GPSPositionTime pt = gps.getBestXYZ();

		REQUIRE(old.posX == pt.posX);
		REQUIRE(old.posY == pt.posY);
		REQUIRE(old.posZ == pt.posZ);
		REQUIRE(old.velX == pt.velX);
		REQUIRE(old.velY == pt.velY);
		REQUIRE(old.velZ == pt.velZ);
		REQUIRE(old.GPSWeek == pt.GPSWeek);
		REQUIRE(old.GPSSec == pt.GPSSec);
	}
	*/


	SECTION("Orbital Propagater","[subsystem][gps]"){
		nm.teststr = teststr;
		gps.fetchNewGPS();
		GPSPositionTime pt = gps.getBestXYZI();

		cout << "POS { " << pt.posX << "," << pt.posY << "," << pt.posZ << " }" << endl;
		cout << "VEL { " << pt.velX << "," << pt.velY << "," << pt.velZ << " }" << endl;

		REQUIRE(fabs(pt.posX - -5763.85) < 0.15);
		REQUIRE(fabs(pt.posY - 0) < 0.15);
		REQUIRE(fabs(pt.posZ - -3753.25) < 0.15);
		REQUIRE(fabs(pt.velX - 4.15403) < 0.15);
		REQUIRE(fabs(pt.velY - 0) < 0.15);
		REQUIRE(fabs(pt.velZ - -6.37933) < 0.15);
	}



}
