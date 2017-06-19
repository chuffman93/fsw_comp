#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "util/Logger.h"
#include "servers/Structs.h"
#include "servers/FileSystem.h"
#include "servers/EPSServer.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

// Note: this test will be invalidated when the EPS config definition changes
TEST(testConfig, testNominal) {
	// create new config
	EPSConfig config(1,2,3);
	uint8 buffer[config.size];
	config.update(buffer,config.size);
	config.serialize();

	// write the new config to a test file
	FILE * fp = fopen(EPS_CFG_UP,"w");
	fwrite(buffer,sizeof(uint8),config.size,fp);
	fclose(fp);

	// update the config
	EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	epsServer->updateConfig();

	remove(EPS_CFG_UP);

	ASSERT_EQ(1,epsServer->EPSConfiguration.minCOMCharge);
	ASSERT_EQ(2,epsServer->EPSConfiguration.minPLDCharge);
	ASSERT_EQ(3,epsServer->EPSConfiguration.minDSTCharge);
}

TEST(testConfig, testEmpty) {
	system("touch " EPS_CFG_UP);

	// update the config
	EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	epsServer->EPSConfiguration.minCOMCharge = 0;
	epsServer->EPSConfiguration.minPLDCharge = 0;
	epsServer->EPSConfiguration.minDSTCharge = 0;
	epsServer->updateConfig();

	remove(EPS_CFG_UP);

	ASSERT_EQ(0,epsServer->EPSConfiguration.minCOMCharge);
	ASSERT_EQ(0,epsServer->EPSConfiguration.minPLDCharge);
	ASSERT_EQ(0,epsServer->EPSConfiguration.minDSTCharge);
}

TEST(testConfig, testPartial) {
	system("touch " EPS_CFG_UP);
	system("echo -n -e '\x00' > " EPS_CFG_UP);

	// update the config
	EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	epsServer->EPSConfiguration.minCOMCharge = 0;
	epsServer->EPSConfiguration.minPLDCharge = 0;
	epsServer->EPSConfiguration.minDSTCharge = 0;
	epsServer->updateConfig();

	remove(EPS_CFG_UP);

	ASSERT_EQ(0,epsServer->EPSConfiguration.minCOMCharge);
	ASSERT_EQ(0,epsServer->EPSConfiguration.minPLDCharge);
	ASSERT_EQ(0,epsServer->EPSConfiguration.minDSTCharge);
}
