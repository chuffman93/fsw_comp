#include "subsystem/CDH.h"


CDH::CDH(std::vector<HotSwapInterface*> * hotswaps, std::vector<PowerMonitorInterface*> * powermonitors, std::vector<TempInterface*> * thermalsensors, OneWireManager* onewire)
: HotSwaps(hotswaps), PowerMonitors(powermonitors), ThermalSensors(thermalsensors), OneWireMan(onewire) {
	tags+= LogTag("Name","CDH");
	health.fileSize = FileManager::MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY CDH_PATH "/CDH";
	state.cpu1 = 0;
	state.cpu5 = 0;
	state.cpu15 = 0;
	state.memory = 0;

}

CDH::~CDH(){

}

/*
 * Initializes hot swaps, power monitors, and temperature monitors
 * Handles CDH configurations
 */
void CDH::initialize(){

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

	handleConfig();

}

/*
 * Inherited function, irrelevant for CDH
 */
void CDH::handleMode(FSWMode transition){}

/*
 * Inherited function, irrelevant for CDH
 */
void CDH::handleConfig(){}

/*
 * Inherited function, irrelevant for CDH
 */
void CDH::updateConfig(){}

CDHState CDH::getCDHState(){
	return this->state;
}


/*
 * Handles CDH health and status data acquisition and storing
 */
void CDH::getHealthStatus(){
	std::vector<HotSwapData> HSHealth = collectHotswap();
	std::vector<PowerMonitorData> PMHealth = collectPowermon();
	std::vector<float> THealth = collectTherm();

	struct sysinfo systemInfo;
	if (sysinfo(&systemInfo) == 0){
		state.cpu1 = systemInfo.loads[0]/6553.6;
		state.cpu5 = systemInfo.loads[1]/6553.6;
		state.cpu15 = systemInfo.loads[2]/6553.6;
		state.memory = 100.0*(259964928.0 - ((float) systemInfo.freeram)) / (259964928.0);
	}else {
		state.cpu1 = 0;
		state.cpu5 = 0;
		state.cpu15 = 0;
		state.memory = 0;
	}

	std::vector<HotSwapData>::iterator it1;
	std::vector<PowerMonitorData>::iterator it2;
	std::vector<float>::iterator it3;

	ByteStream bs;

	bs << state.cpu1 << state.cpu5 << state.cpu15 << state.memory;

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

/*
 * Inherited function, Irrelevant for CDH
 * Handles sending opCodes
 * \param opcode to be sent
 * \param buffer to be sent if needed
 */
ACPPacket CDH::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	ACPPacket packet;
	return packet;
}

//! Handles collecting hot swap data
std::vector<HotSwapData> CDH::collectHotswap(){
	std::vector<HotSwapInterface*>::iterator it;
	std::vector<HotSwapData> data;

	Logger::Stream(LEVEL_INFO,tags) << "Collecting Hot Swap Data";
	for (it = HotSwaps->begin(); it != HotSwaps->end(); it++){
		data.push_back((*it)->getData());
	}

	return data;
}

//! Handles collecting Power Monitors
std::vector<PowerMonitorData> CDH::collectPowermon(){
	std::vector<PowerMonitorInterface*>::iterator it;
	std::vector<PowerMonitorData> data;

	Logger::Stream(LEVEL_INFO,tags) << "Collecting Power Monitor Data";
	for (it = PowerMonitors->begin(); it != PowerMonitors->end(); it++){
		data.push_back((*it)->getData());
	}
	return data;

}

//! Handles collecting Temperature Monitors
std::vector<float> CDH::collectTherm(){
	std::vector<TempInterface*>::iterator it;
	std::vector<float> data;

	Logger::Stream(LEVEL_INFO,tags) << "Collecting Temperature Sensor Data";
	for (it = ThermalSensors->begin(); it != ThermalSensors->end(); it++){
		(*it)->beginSample();
	}
	usleep(1000*1000);
	for (it = ThermalSensors->begin(); it != ThermalSensors->end(); it++){
		data.push_back((*it)->getSample());
	}

	return data;
}

