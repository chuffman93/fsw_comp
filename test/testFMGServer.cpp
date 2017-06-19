#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "servers/FMGServer.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

//TEST(DISABLED_TestFMGServer, testPacketDestinations){
//	//Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON);
//	FMGServer * fmgServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
//	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	WatchdogManager::StartServer(fmgServer, 0,	true);
//
//	for (int iter = 0; iter < 10; iter++) {
//		string str = "12345678901234567890123456789012345678901234567890";
//		fmgServer->Log(DESTINATION_CMD, str);
//		fmgServer->Log(DESTINATION_HST, str);
//		fmgServer->Log(DESTINATION_MOD, str);
//		fmgServer->Log(DESTINATION_SWP, str);
//		fmgServer->Log(DESTINATION_ERR, str);
//		fmgServer->Log(DESTINATION_DGN, str);
//		fmgServer->Log(DESTINATION_GEN, str);
//		fmgServer->Log(DESTINATION_FSS, str);
//		fmgServer->Log(DESTINATION_SSS, str);
//		fmgServer->Log(DESTINATION_RAD, str);
//	}
//
//	while (fmgServer->FileQueue.size() > 0) {
//		usleep(100000);
//	}
//
//	// Need to manually verify that files exist
//
//}
//
//TEST(DISABLED_TestFMGServer, testNoDestination){
//	FMGServer * fmgServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
//
//	for (int iter = 0; iter < 10; iter++) {
//		FilePacket myPacket1;
//		myPacket1.buffer = "no dest specified";
//		fmgServer->FileQueue.push(myPacket1);
//	}
//
//	while (fmgServer->FileQueue.size() > 0) {
//			usleep(100000);
//	}
//
//	// Need to manually verify that no files were created
//}
//
//TEST(DISABLED_TestFMGServer, testLongMessage){
//	FMGServer * fmgServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
//
//	for (int iter = 0; iter < 10; iter++) {
//		FilePacket myPacket11;
//		myPacket11.buffer = "this is a very long message. There is lots and lots and lots of data here! Oh no! What am I possibly going to do with all this data?? Who knows? But one thing is for sure... There is a hell of a lot of data here. Jeez, it just keeps on coming doesn't it. Yessir. Just keeps on coming. Why are you still reading this buffer anyway? Don't you have better things to be doing than reading a buffer??? Okay, I'm done.";
//		myPacket11.dest = DESTINATION_GEN;
//		fmgServer->FileQueue.push(myPacket11);
//	}
//
//	while (fmgServer->FileQueue.size() > 0) {
//			usleep(100000);
//	}
//
//	// Need to manually verify that files exist
//}
