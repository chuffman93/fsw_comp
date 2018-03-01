#include "subsystem/CDH.h"


CDH::CDH(std::vector<HotSwapInterface*> * hotswaps, std::vector<PowerMonitorInterface*> * powermonitors, std::vector<TempInterface*> * thermalsensors, OneWireManager* onewire)
: HotSwaps(hotswaps), PowerMonitors(powermonitors), ThermalSensors(thermalsensors), OneWireMan(onewire) {
	tags+= LogTag("Name","CDH");
	health.fileSize = MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY CDH_PATH "/CDH";
}

CDH::~CDH(){

}

//Will set up the Gpio lines and the acp devices
bool CDH::initialize(){

	std::vector<HotSwapInterface*>::iterator it1;
	std::vector<PowerMonitorInterface*>::iterator it2;

	Logger::Stream(LEVEL_INFO,tags) << "Initializing Hot Swaps";
	for (it1 = HotSwaps->begin(); it1 != HotSwaps->end(); it1++){
		(*it1)->configure();
	}
	Logger::Stream(LEVEL_INFO,tags) << "Initializing Power Monitors";
	for (it2 = PowerMonitors->begin(); it2 != PowerMonitors->end(); it2++){
		(*it2)->configure();
	}
	Logger::Stream(LEVEL_INFO,tags) << "Initializing Temperature Monitors";
	OneWireMan->initialize();

	return true;
}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void CDH::handleMode(FSWMode transition){

}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void CDH::getHealthStatus(){
	std::vector<HotSwapData> HSHealth = collectHotswap();
	std::vector<PowerMonitorData> PMHealth = collectPowermon();
	std::vector<float> THealth = collectTherm();

	std::vector<HotSwapData>::iterator it1;
	std::vector<PowerMonitorData>::iterator it2;
	std::vector<float>::iterator it3;

	ByteStream bs;

	for (it1 = HSHealth.begin(); it1 != HSHealth.end(); it1++){
		bs << (*it1).current << (*it1).voltage;
	}

	for (it2 = PMHealth.begin(); it2 != PMHealth.end(); it2++){
		bs << (*it2).current << (*it2).voltage;
	}

	for (it3 = THealth.begin(); it3 != THealth.end(); it3++){
		bs << (*it3);
	}

	std::vector<uint8_t> buff = bs.vec();
	health.recordBytes(buff);

}

ACPPacket CDH::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	ACPPacket packet;
	return packet;
}

//Various methods for data collection
std::vector<HotSwapData> CDH::collectHotswap(){
	std::vector<HotSwapInterface*>::iterator it;
	std::vector<HotSwapData> data;

	Logger::Stream(LEVEL_INFO,tags) << "Collecting Hot Swap Data";
	for (it = HotSwaps->begin(); it != HotSwaps->end(); it++){
		data.push_back((*it)->getData());
	}

	return data;
}

std::vector<PowerMonitorData> CDH::collectPowermon(){
	std::vector<PowerMonitorInterface*>::iterator it;
	std::vector<PowerMonitorData> data;

	Logger::Stream(LEVEL_INFO,tags) << "Collecting Power Monitor Data";
	for (it = PowerMonitors->begin(); it != PowerMonitors->end(); it++){
		data.push_back((*it)->getData());
	}
	return data;

}

std::vector<float> CDH::collectTherm(){
	std::vector<TempInterface*>::iterator it;
	std::vector<float> data;

	Logger::Stream(LEVEL_INFO,tags) << "Collecting Temperature Sensor Data";
	for (it = ThermalSensors->begin(); it != ThermalSensors->end(); it++){
		(*it)->beginSample();
		usleep(1000*1000);
		data.push_back((*it)->getSample());
	}

	return data;
}

