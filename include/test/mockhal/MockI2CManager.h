/*
 * MockI2CManager.h
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#ifndef MOCKI2CMANAGER_H_
#define MOCKI2CMANAGER_H_

#include <map>
#include <vector>
#include "hal/I2CManager.h"

struct MockI2CDevice{
	std::map<uint8_t, uint8_t> registers;
	std::vector<uint8_t> rawread;
	std::vector<uint8_t> rawwrite;

	bool operator ==(const MockI2CDevice& dev){return false;}
};

class MockI2CManager: public I2CManager, public BusManager<MockI2CDevice>{
public:
	MockI2CManager():I2CManager(""){}
	~MockI2CManager(){}

	int attachDevice(uint8_t addr){
		MockI2CDevice dev;
		return BusManager<MockI2CDevice>::attachDevice(dev);
	}
	void initialize(){};
	void initializeDevice(MockI2CDevice &dev) {};
	uint8_t readReg(int id, uint8_t reg){
		MockI2CDevice& dev = BusManager<MockI2CDevice>::getDevice(id);
		std::map<uint8_t, uint8_t>::iterator i = dev.registers.find(reg);
		if(i != dev.registers.end()){
			return dev.registers[reg];
		}
		return 0;
	}
	void writeReg(int id, uint8_t reg, uint8_t value){
		MockI2CDevice& dev = BusManager<MockI2CDevice>::getDevice(id);
		dev.registers[reg] = value;
	}
	std::vector<uint8_t> readRaw(int id, size_t len){
		MockI2CDevice& dev = BusManager<MockI2CDevice>::getDevice(id);
		return dev.rawread;
	}
	void writeRaw(int id, std::vector<uint8_t> data){
		MockI2CDevice& dev = BusManager<MockI2CDevice>::getDevice(id);
		dev.rawwrite = data;
	}
	void setRead(int id, std::vector<uint8_t> data){
		MockI2CDevice& dev = BusManager<MockI2CDevice>::getDevice(id);
		dev.rawread = data;
	}
	std::vector<uint8_t> getWritten(int id){
		MockI2CDevice& dev = BusManager<MockI2CDevice>::getDevice(id);
		return dev.rawwrite;
	}
};


#endif /* MOCKI2CMANAGER_H_ */
