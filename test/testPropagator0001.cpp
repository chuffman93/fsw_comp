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

double ecefPos[3] = {5197.70461, 4504.72374, 11.860507};
double ecefVel[3] = {0.318377, -0.387397, 7.612595};
double eciPos[3] = {6878.14, 0, 0};
double eciVel[3] = {0, 0, 7.612607};
double gpsTime[2] = {1981, 140418};

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

TEST(TestPropagator, testECEFtoECI) {
	double eciResultPos[3];
	double eciResultVel[3];
	wgs2gcrf(ecefPos, ecefVel, gpsTime, eciResultPos, eciResultVel);

	printf("%f, %f, %f\n%f, %f, %f\n", eciResultPos[0], eciResultPos[1], eciResultPos[2], eciResultVel[0], eciResultVel[1], eciResultVel[2]);

	ASSERT_TRUE(6878.1 < eciResultPos[0] && eciResultPos[0] < 6878.2);
	ASSERT_TRUE(-0.1 < eciResultPos[1] && eciResultPos[1] < 0.1);
	ASSERT_TRUE(-0.1 < eciResultPos[2] && eciResultPos[2] < 0.1);
	ASSERT_TRUE(-0.1 < eciResultVel[0] && eciResultVel[0] < 0.1);
	ASSERT_TRUE(-0.1 < eciResultVel[1] && eciResultVel[1] < 0.1);
	ASSERT_TRUE(7.612 < eciResultVel[2] && eciResultVel[2] < 7.613);
}

TEST(TestPropagator, testECItoECEF) {
	double ecefResultPos[3];
	double ecefResultVel[3];
	gcrf2wgs(eciPos, eciVel, gpsTime, ecefResultPos, ecefResultVel);

	printf("%f, %f, %f\n%f, %f, %f\n", ecefResultPos[0], ecefResultPos[1], ecefResultPos[2], ecefResultVel[0], ecefResultVel[1], ecefResultVel[2]);

	ASSERT_TRUE(5197.7 < ecefResultPos[0] && ecefResultPos[0] < 5197.8);
	ASSERT_TRUE(4504.7 < ecefResultPos[1] && ecefResultPos[1] < 4504.8);
	ASSERT_TRUE(11.860 < ecefResultPos[2] && ecefResultPos[2] < 11.861);
	ASSERT_TRUE(0.3183 < ecefResultVel[0] && ecefResultVel[0] < 0.3184);
	ASSERT_TRUE(-0.3874 < ecefResultVel[1] && ecefResultVel[1] < -0.3873);
	ASSERT_TRUE(7.612 < ecefResultVel[2] && ecefResultVel[2] < 7.613);
}
