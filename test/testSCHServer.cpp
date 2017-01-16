#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "servers/SCHServer.h"
#include <fstream>


using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;



TEST(TestSCHServer, testParse){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	SCHServer::SCHItem item = schServer->ParseLine("1.2345672.3456783.4567891234234534561\n");
	ASSERT_EQ(item.latitude, 1.234567);
	ASSERT_EQ(item.longitude, 2.345678);
	ASSERT_EQ(item.radius, 3.456789);
	ASSERT_EQ(item.duration, 1234);
	ASSERT_EQ(item.entry_timeout, 2345);
	ASSERT_EQ(item.skip_timeout, 3456);
	ASSERT_EQ(item.mode, 1);
}

TEST(TestSCHServer, testSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");

	ofstream file;
	file.open(SCH_SCHEDULE_FILE);

	file << "1.2345672.3456783.4567891234234534561\n";
	file << "9.8765438.7654327.6543219876876576541\n";

	file.close();

	int err = schServer->LoadNextSchedule();
	ASSERT_EQ(err, 1);

	SCHServer::SCHItem sched1 = schServer->currentSchedule.front();
	ASSERT_EQ(sched1.latitude, 1.234567);
	ASSERT_EQ(sched1.longitude, 2.345678);
	ASSERT_EQ(sched1.radius, 3.456789);
	ASSERT_EQ(sched1.duration, 1234);
	ASSERT_EQ(sched1.entry_timeout, 2345);
	ASSERT_EQ(sched1.skip_timeout, 3456);
	ASSERT_EQ(sched1.mode, 1);
	schServer->currentSchedule.pop_front();

	SCHServer::SCHItem sched2 = schServer->currentSchedule.front();
	ASSERT_EQ(sched2.latitude, 9.876543);
	ASSERT_EQ(sched2.longitude, 8.765432);
	ASSERT_EQ(sched2.radius, 7.654321);
	ASSERT_EQ(sched2.duration, 9876);
	ASSERT_EQ(sched2.entry_timeout, 8765);
	ASSERT_EQ(sched2.skip_timeout, 7654);
	ASSERT_EQ(sched2.mode, 1);
	schServer->currentSchedule.pop_front();

	remove(SCH_SCHEDULE_FILE);
	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}


TEST(TestSCHServer, testNoSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");

	int err = schServer->LoadNextSchedule();
	ASSERT_EQ(err, -1);

	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}


TEST(TestSCHServer, testBadSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");
	ofstream file;
	file.open(SCH_SCHEDULE_FILE);
	file << "asdf";

	file.close();

	int err = schServer->LoadNextSchedule();
	ASSERT_EQ(err, -3);

	remove(SCH_SCHEDULE_FILE);
	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}
