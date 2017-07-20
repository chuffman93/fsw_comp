#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "servers/GPSServer.h"
#include "servers/GPSStdTasks.h"

extern "C" {
	#include "util/propagator/AllStarOrbitProp.h"
	#include "util/propagator/gpsFrameRotation.h"
}

#define DEG2RAD     M_PI/180

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

TEST(TestPropagator, testTimeUpdate) {
	uint32 secondsFromEpoch;
	ConvertToEpochTime(1957, 441600, &secondsFromEpoch);
	ASSERT_EQ(1500000000, secondsFromEpoch);
}

TEST(TestPropagator, testTimeConvert) {
	uint16 GPSWeek;
	float GPSSec;
	ConvertToGPSTime(1500000000, &GPSWeek, &GPSSec);

	ASSERT_EQ(1957, GPSWeek);
	ASSERT_EQ(441600, GPSSec);
}
