#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "servers/SCHServer.h"


using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;



TEST(TestSCHServer, testParse){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	WatchdogManager::StartServer(schServer, 0,	true);

	SCHItem item = schServer.parseLine("1.2345672.3456783.45678912341\n")
	ASSERT_EQ(item.latitude, 1.234567);
	ASSERT_EQ(item.longitude, 2.345678);
	ASSERT_EQ(item.radius, 3.456789);
	ASSERT_EQ(item.timeoutms, 1234);
	ASSERT_EQ(item.mode, 1);
}

TEST(TestSCHServer, testSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	WatchdogManager::StartServer(schServer, 0,	true);

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");

	ofstream file (SCH_SCHEDULE_FILE);

	file << "1.2345672.3456783.45678912341\n";
	file << "9.8765438.7654327.65432198769\n";

	int err = schServer.LoadNextSchedule();
	ASSERT_EQ(err, 1);

	SCHItem sched1 = schServer.GetSCHItem(0);
	ASSERT_EQ(sched1.latitude, 1.234567);
	ASSERT_EQ(sched1.longitude, 2.345678);
	ASSERT_EQ(sched1.radius, 3.456789);
	ASSERT_EQ(sched1.timeoutms, 1234);
	ASSERT_EQ(sched1.mode, 1);

	SCHItem sched1 = schServer.GetSCHItem(1);
	ASSERT_EQ(sched2.latitude, 9.876543);
	ASSERT_EQ(sched2.longitude, 8.765432);
	ASSERT_EQ(sched2.radius, 7.654321);
	ASSERT_EQ(sched2.timeoutms, 9876);
	ASSERT_EQ(sched2.mode, 9);

	file.close();

	remove(SCH_SCHEDULE_FILE);
	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}


TEST(TestSCHServer, testNoSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	WatchdogManager::StartServer(schServer, 0,	true);

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");

	int err = schServer.LoadNextSchedule();
	ASSERT_EQ(err, -1);

	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}


TEST(TestSCHServer, testBadSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	WatchdogManager::StartServer(schServer, 0,	true);

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");
	ofstream file (SCH_SCHEDULE_FILE);
	file << "asdf";

	int err = schServer.LoadNextSchedule();
	ASSERT_EQ(err, -2);

	file.close();
	remove(SCH_SCHEDULE_FILE);
	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}
