/*
 * test_GPStoACS.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: fsw
 */
#include "test/catch.hpp"
#include "test/testmacros.h"
#include "subsystem/GPS.h"
#include "core/Architecture.h"
#include "test/swintegration/MockSubPower.h"
#include "test/swintegration/MockNMEA.h"
#include <unistd.h>
#include <fstream>
#include "util/TimeKeeper.h"

std::string teststr1 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140400.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-4504732.097,-5197706.929,1.113,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,0.002,0,7612.608,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr2 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140700.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-4258687.443,-4913813.007,2242050.754,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,1625.199,1875.206,7196.814,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr3 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141000.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-3547431.835,-4093143.454,4239183.015,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,3072.862,3545.567,5994.854,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr4 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141300.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-2448661.489,-2825346.658,5773234.948,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4184.852,4828.618,4138.027,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr5 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141600.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1082403.997,-1248914.342,6676629.625,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4839.696,5584.199,1829.17,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr6 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141900.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,402093.258,463947.013,6850681.92,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4965.861,5729.773,-679.502,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr7 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,142200.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,1842666.608,2126127.674,6276378.691,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4549.565,5249.438,-3113.946,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr8 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,142500.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,3081950.545,3556054.173,5016455.736,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,3636.283,4195.664,-5208.229,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr9 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,142800.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,3984568.039,4597524.046,3208544.652,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,2325.781,2683.563,-6733.575,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr10 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,143100.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,4451918.863,5136769.095,1050138.225,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,761.214,878.315,-7523.358,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr11 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,143400.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,4432950.499,5114883.213,-1222983.315,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-886.506,-1022.878,-7491.304,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr12 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,143700.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,3929735.013,4534257.171,-3362508.472,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-2437.386,-2812.334,-6640.913,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
std::string teststr13 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,144000.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,2997242.715,3458317.459,-5134719.569,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-3722.01,-4294.576,-5065.082,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";


TEST_CASE("Same test different string","[gps2acs][t2]"){
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_INFO);
	Logger::registerFilter(LogTag("Name", "GPS"), LEVEL_DEBUG);
	Architecture::setInterfaceMode(SOFTWARE);
	Architecture::buildACS();
	ACS *acs = Architecture::getACS();

	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	acs->initialize();

	acs->pointSunSoak();
	std::string teststr = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140418.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-210817.835999999,-5956876.66,3432241.469,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,122.524,3797.68,6597.06,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	nm.setString(teststr);
	gps.fetchNewGPS();
	GPSPositionTime pt = gps.getBestXYZI();
	Logger::Stream(LEVEL_INFO) <<  pt;
	for(int y = 0; y <= 300; y++){
		acs->sendGPS(gps.getBestXYZI());
	}
	REQUIRE(fabs(pt.posX - -4045.750977) < 0.01);
	REQUIRE(fabs(pt.posY - -4363.587891) < 0.01);
	REQUIRE(fabs(pt.posZ - 3493.069580) < 0.01);
	REQUIRE(fabs(pt.velX - 2.909381) < 0.01);
	REQUIRE(fabs(pt.velY - 2.493242) < 0.01);
	REQUIRE(fabs(pt.velZ - 6.592149) < 0.01);
	REQUIRE(pt.GPSWeek == 1981);
	REQUIRE(pt.GPSSec  == 140418);

	acs->pointNadir();
	teststr = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140400.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,0,-6878132.838,-7566.621,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-10.113,-8.375,7612.597,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	nm.setString(teststr);
	gps.fetchNewGPS();
	for(int y = 0; y <= 300; y++){
		acs->sendGPS(gps.getBestXYZI());
		if(y%60 == 0){
			acs->getHealthStatus();
		}
	}

	acs->pointCOM();
	teststr = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140400.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,0,-6878132.838,-7566.621,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,-10.113,-8.375,7612.597,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	nm.setString(teststr);
	gps.fetchNewGPS();
	for(int y = 0; y <= 300; y++){
		acs->sendGPS(gps.getBestXYZI());
		if(y%60 == 0){
			acs->getHealthStatus();
		}
	}
}



TEST_CASE("Test ACS's GPS handling","[.][gps2acs]"){

	Architecture::buildTime();
	MockNMEA nm;
	GPS gps(nm,*(new MockSubPower("GPS")));
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_INFO);
	Logger::registerFilter(LogTag("Name", "GPS"), LEVEL_DEBUG);
	Architecture::setInterfaceMode(HARDWARE);
	Architecture::buildACS();
	ACS *acs = Architecture::getACS();

	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	acs->initialize();

	acs->pointSunSoak();
	std::string tststrs[6] = {teststr1,teststr2,teststr3,teststr4,teststr5,teststr6};
	for(int x = 0; x < 6; x++){
		nm.setString(tststrs[x]);
		gps.fetchNewGPS();
		for(int y = 0; y <= 300; y++){
			acs->sendGPS(gps.getBestXYZI());
			if(y%60 == 0){
				acs->getHealthStatus();
			}
		}
	}

	acs->pointNadir();

	teststr1 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140400.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-4504732.097,-5197706.929,1.113,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,0.002,0,7612.608,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr2 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140700.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-4258687.443,-4913813.007,2242050.754,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,1625.199,1875.206,7196.814,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr3 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141000.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-3547431.835,-4093143.454,4239183.015,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,3072.862,3545.567,5994.854,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr4 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141300.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-2448661.489,-2825346.658,5773234.948,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4184.852,4828.618,4138.027,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr5 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141600.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1082403.997,-1248914.342,6676629.625,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4839.696,5584.199,1829.17,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr6 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141900.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,402093.258,463947.013,6850681.92,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4965.861,5729.773,-679.502,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";

	std::string tststrs1[6] = {teststr1,teststr2,teststr3,teststr4,teststr5,teststr6};
	for(int x = 0; x < 6; x++){
		nm.setString(tststrs1[x]);
		gps.fetchNewGPS();
		for(int y = 0; y <= 300; y++){
			acs->sendGPS(gps.getBestXYZI());
			if(y%60 == 0){
				acs->getHealthStatus();
			}
		}
	}

	acs->pointCOM();

	teststr1 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140400.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-4504732.097,-5197706.929,1.113,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,0.002,0,7612.608,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr2 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,140700.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-4258687.443,-4913813.007,2242050.754,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,1625.199,1875.206,7196.814,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr3 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141000.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-3547431.835,-4093143.454,4239183.015,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,3072.862,3545.567,5994.854,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr4 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141300.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-2448661.489,-2825346.658,5773234.948,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4184.852,4828.618,4138.027,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr5 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141600.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1082403.997,-1248914.342,6676629.625,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4839.696,5584.199,1829.17,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
	teststr6 = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141900.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,402093.258,463947.013,6850681.92,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,4965.861,5729.773,-679.502,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";

	std::string tststrs2[6] = {teststr1,teststr2,teststr3,teststr4,teststr5,teststr6};
	for(int x = 0; x < 6; x++){
		nm.setString(tststrs2[x]);
		gps.fetchNewGPS();
		for(int y = 0; y <= 300; y++){
			acs->sendGPS(gps.getBestXYZI());
			if(y%60 == 0){
				acs->getHealthStatus();
			}
		}
	}

}
