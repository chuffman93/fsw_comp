/*
 * OneWireManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "hal/OneWireManager.h"
#include "util/EventHandler.h"
#include <fstream>
#include <sstream>
using namespace std;

/*!
 * Creates one wire manager on the provided bus
 * \param busbase the filepath the root directory of the bus
 */
OneWireManager::OneWireManager(string busbase)
:busbase(busbase)
{}

//! Does nothing
OneWireManager::~OneWireManager(){}

/*!
 * Attach a device with the provided name to the bus
 * \param devname the name of the device
 * \return the id of the device
 */
int OneWireManager::attachDevice(string devname){
	OneWireDevice dev;
	dev.devname = devname + "/";
	return BusManager<OneWireDevice>::attachDevice(dev);
}

//! Initialize the bus
void OneWireManager::initialize(){
	EventHandler::event(LEVEL_INFO, "[OneWireManager] Initializing one wire bus at " + busbase);
	BusManager<OneWireDevice>::initializeDevices();
}

/*!
 * Initialize a device on the bus
 * \param dev the device to be initialized
 */
void OneWireManager::initializeDevice(OneWireDevice& dev){
	EventHandler::event(LEVEL_INFO, "[OneWireManager] Initializing one wire device " + dev.devname + " on bus " + busbase);
}

/*!
 * Writes the provided data to the device
 * \param id the id of the device
 * \param filename the file in the device's directory to write to
 * \param data the string of data to be written
 */
void OneWireManager::writeToFile(int id, std::string filename, std::string data){
	LockGuard l(lock);
	OneWireDevice& dev = BusManager<OneWireDevice>::getDevice(id);
	EventHandler::event(LEVEL_DEBUG, "[OneWireManager] Writing \"" + data + "\" to " + busbase + dev.devname + filename);
	ofstream fs;
	fs.open((busbase + dev.devname + filename).c_str());
	fs << data;
	fs.close();
}

/*!
 * Reads the data in a file
 * \param id the id of the device
 * \param filename the file in the device's directory to read from
 * \return the data that was read
 */
std::string OneWireManager::readFromFile(int id, std::string filename){
	LockGuard l(lock);
	OneWireDevice& dev = BusManager<OneWireDevice>::getDevice(id);
	EventHandler::event(LEVEL_DEBUG, "[OneWireManager] Reading from " + busbase + dev.devname + filename);
	ifstream fs;
	//The use of stringstream lets us get endlines as well... kinda hack but it works
	stringstream data;
	fs.open((busbase + dev.devname + filename).c_str());
	data << fs.rdbuf();
	fs.close();
	return data.str();
}

