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

TEST(TestPropagator, testTimeUpdate){
	UpdateTime(1947, 172800);
	ASSERT_TRUE(true);
}
