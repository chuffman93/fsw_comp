/*
 * MockOneWIreManager.h
 *
 *  Created on: Dec 28, 2017
 *      Author: cyborg9
 */

#ifndef MOCKONEWIREMANAGER_H_
#define MOCKONEWIREMANAGER_H_

#include "hal/OneWireManager.h"

struct MockOneWireDevice{
	std::string datastring;

	bool operator ==(const MockOneWireDevice& dev){return false;}
};

class MockOneWireManager: public OneWireManager, public BusManager<MockOneWireDevice>{
public:
	MockOneWireManager():OneWireManager(""){}
	~MockOneWireManager(){}

	int attachDevice(std::string devname){
		MockOneWireDevice dev;
		return BusManager<MockOneWireDevice>::attachDevice(dev);
	}
	void initialize(){};

	void writeToFile(int id, std::string file, std::string data) {}

	std::string readFromFile(int id, std::string file){
		return BusManager<MockOneWireDevice>::getDevice(id).datastring;
	}

	void setData(int id, std::string data){
		BusManager<MockOneWireDevice>::getDevice(id).datastring = data;
	}

};




#endif /* MOCKONEWIREMANAGER_H_ */
