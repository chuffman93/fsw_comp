#include "test/swintegration/MockNMEA.h"

static UARTManager dummyu("");

MockNMEA::MockNMEA()
:NMEAInterface(dummyu)
{
	tags += LogTag("Name", "MockNMEA");
}
MockNMEA::~MockNMEA(){}

void MockNMEA::setString(std::string fakeGPS){
	Logger::Stream(LEVEL_DEBUG, tags) << "setting \"" << fakeGPS << "\"";
	teststr = fakeGPS;

}

std::string MockNMEA::getString(){
	Logger::Stream(LEVEL_DEBUG, tags) << "getting \"" << teststr << "\"";
	return teststr;
}

void MockNMEA::sendCommand(std::string str){
	Logger::Stream(LEVEL_INFO, tags) << "Sending \"" << str << "\"";
}
