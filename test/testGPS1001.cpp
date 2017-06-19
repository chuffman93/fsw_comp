#include "gtest/gtest.h"
#include "core/StdTypes.h"
#include "core/Factory.h"

#include "servers/GPSStdTasks.h"
#include "servers/GPSServer.h"
#include <string>

using namespace std;
using namespace rel_ops;
using namespace AllStar::Servers;
using namespace AllStar::Core;


TEST(TestGPS, testDataParse){
	char testBuffer[281] = "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,00000040,d821,2724;"
		"SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,"
		"0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,"
		"0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca\r\n";

	GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));

	ASSERT_TRUE(BESTXYZProcess(testBuffer, 281));
	ASSERT_EQ(gpsServer->GetGPSDataPtr()->numTracked, 12);

}

TEST(TestGPS, testCoordsParse){
	char testBuffer[81] = "GPRMC,144326.00,A,5107.0017737,N,11402.3291611,W,0.080,323.3,210307,0.0,E,A*20\r\n";

	GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));

	ASSERT_TRUE(GPRMCProcess(testBuffer, 81));
	ASSERT_EQ(51.1167, gpsServer->GetGPSCoordsPtr()->latitude);
	ASSERT_EQ(-114.039, gpsServer->GetGPSCoordsPtr()->longitude);
}

TEST(TestGPS, testCRC){
	char * i = (char *) "BESTPOSA,COM1,0,78.0,FINESTEERING,1427,325298.000,00000000,6145,2748;SOL_COMPUTED,SINGLE,51.11678928753,-114.03886216575,1064.3470,-16.2708,WGS84,2.3434,1.3043,4.7300,\"\",0.000,0.000,7,7,0,0,0,06,0,03";
	uint32 CRC = CalculateCRC_GPS(i);

	ASSERT_EQ(CRC, 0x9c9a92bb);
}

TEST(TestGPS, testChecksum){
	char * i = (char *) "GPRMC,144326.00,A,5107.0017737,N,11402.3291611,W,0.080,323.3,210307,0.0,E,A";

	uint8 checksum = CalculateNMEAChecksum(i);

	ASSERT_EQ(checksum, 0x20);
}
