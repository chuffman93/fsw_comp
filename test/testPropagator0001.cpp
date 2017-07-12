#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "servers/GPSServer.h"

extern "C" {
	#include "util/propagator/AllStarOrbitProp.h"
	#include "util/propagator/gpsFrameRotation.h"
}

#define DEG2RAD     M_PI/180

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

TEST(TestPropagator, testPropagate){
	// Sample GPS data (frame-shifted to ECI)
	float gpsPos_eci[3];
	gpsPos_eci[0] = 3854.398661;
	gpsPos_eci[1] = 4541.496164;
	gpsPos_eci[2] = 3439.0685;
	float gpsVel_eci[3];
	gpsVel_eci[0] = 2.462967;
	gpsVel_eci[1] = 2.902024;
	gpsVel_eci[2] = -6.592712;
	float GPS_TOW = 518400;
	float GPS_Week = 1767;
	float last_GPS_TOW = 518400;

	// Store the result in these arrays
	float satPos_eci[3] = { 0.0, 0.0, 0.0 };
	float satVel_eci[3] = { 0.0, 0.0, 0.0 };

	// unused sample updated ephemeris
	int newEphemeris = 0; // not new
	classicElements new_NORAD_eph_coe;
	new_NORAD_eph_coe.a = 6698.140;
	new_NORAD_eph_coe.e = 0.0029859;
	new_NORAD_eph_coe.i = 51.7 * DEG2RAD;
	new_NORAD_eph_coe.Omega = -100 * DEG2RAD;
	new_NORAD_eph_coe.omega = 25 * DEG2RAD;
	new_NORAD_eph_coe.anom = 0;

	// 24 minutes since last lock
	float sysTime = 24 * 60;

	propagatePositionVelocity(gpsPos_eci, gpsVel_eci, GPS_TOW, &last_GPS_TOW, &sysTime, &newEphemeris, &new_NORAD_eph_coe, satPos_eci, satVel_eci);
	printf("Position:\nx: %f\ny: %f\nz: %f\n", satPos_eci[0], satPos_eci[1], satPos_eci[2]);
	printf("Velocity:\nx: %f\ny: %f\nz: %f\n", satVel_eci[0], satVel_eci[1], satVel_eci[2]);

	ASSERT_TRUE(true);
}
