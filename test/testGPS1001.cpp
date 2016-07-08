#include "gtest/gtest.h"
#include "core/StdTypes.h"
#include "core/Factory.h"
#include "servers/GPSStdTasks.h"
#include "servers/GPSServer.h"
#include <string>

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Servers;
using namespace Phoenix::Core;


TEST(TestGPS, testParse){
	char testBuffer[280] = "#BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,00000040,d821,2724;"
			"SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,"
			"0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,"
			"0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";

	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));

	ASSERT_TRUE(GPSDataProcess(testBuffer, 280));
	ASSERT_EQ(gpsServer->GetGPSDataPtr()->numTracked, 12);

}
