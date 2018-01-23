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

std::string teststr = "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";


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

		/*
		cout << "Read: Position = {" << pt.posX << ", " << pt.posY << ", " << pt.posZ << "}" << endl;
		cout << "Read: Velocity = {" << pt.velX << ", " << pt.velY << ", " << pt.velZ << "}" << endl;
		cout << "Read: Week " << pt.GPSWeek << " Second" << pt.GPSSec << endl;
		*/

		REQUIRE(fabs(pt.posX - -1634.53) < 0.01);
		REQUIRE(fabs(pt.posY - -3664.62) < 0.01);
		REQUIRE(fabs(pt.posZ -   4942.5) < 0.01);
		REQUIRE(fabs(pt.velX -  1.1e-06) < 0.01);
		REQUIRE(fabs(pt.velY - -4.9e-06) < 0.01);
		REQUIRE(fabs(pt.velZ -   -1e-07) < 0.01);
		REQUIRE(pt.GPSWeek == 1419);
		REQUIRE(pt.GPSSec  == 340033);
	}

	SECTION("Test that it doesn't set when the string is invalid"){
		GPSPositionTime old = gps.getBestXYZ();
		nm.teststr = "B" + teststr; //Give an invalid char at the front
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

	SECTION("Convert to ECI","[subsytem][gps]"){
		GPSPositionTime pt = gps.getBestXYZ();
		std::string teststr = "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";


	}

	SECTION("Orbital Propagater","[subsystem][gps]"){

	}

}
