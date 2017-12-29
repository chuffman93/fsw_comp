/*
 * GPIOManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include "hal/GPIOManager.h"

//! Stub, not yet implemented
GPIOManager::GPIOManager(std::string busbase)
:gpiobase(busbase)
{
	tags += LogTag("Name", "GPIOManager");
}

//! Stub, not yet implemented
GPIOManager::~GPIOManager(){

}

//! No general setup for the bus, only calls initializer for devices
void GPIOManager::initialize(){
	BusManager<GPIODevice>::initializeDevices();
}

//! Initialize the passed gpio. performs muxings and other tasks
void GPIOManager::initializeDevice(GPIODevice& dev){
	Logger::Stream(LEVEL_INFO, tags) << "Initializing GPIO on " << dev.gpioref
			<< " to be an " << ((dev.direction == GPIO_INPUT) ? "INPUT" : "OUTPUT")
			<< ((dev.edge == INT_NONE) ? "" : " Edge triggered");
	std::ofstream fs;
	struct stat st;
	//If the gpio isn't already exported export it
	if(stat((gpiobase + dev.gpioref).c_str(), &st) != 0){
		fs.open((gpiobase + "export").c_str());
		fs << dev.gpionum;
		fs.close();
	}

	//Set the output mode
	// TODO: check error
	fs.open((gpiobase + dev.gpioref + "direction").c_str());
	fs << ((dev.direction == GPIO_INPUT) ? "in" : "out");
	fs.close();

	//Set the edge and initialize poll fd if appropriate
	if(dev.edge != INT_NONE){
		fs.open((gpiobase + dev.gpioref + "edge").c_str());
		fs << ((dev.edge == INT_RISING) ? "rising" : "falling");
		fs.close();

		dev.pfd.fd = open((gpiobase + dev.gpioref + "value").c_str(), O_RDONLY);
		dev.pfd.events = POLLPRI;

		//Clear any pending interrupts
		uint8_t dummy;
		read(dev.pfd.fd, &dummy, 1);
	}
}

/*! Parses the inputs into those needed by initialize and the other methods to operate on the gpio
 * \param bank The bank of the gpio. Can be A, B, C, D, or E
 * \param pin The pin in the bank to use. Can be between 0 and 31
 * \param direction The direction of the GPIO (in/out)
 * \return the id of the registered device
 */
int GPIOManager::attachDevice(char bank, int pin, GPIODirection direction){
	assert(bank >= 'A');
	assert(bank <= 'E');
	assert(pin >= 0);
	assert(pin <= 31);
	GPIODevice dev;
	dev.direction = direction;
	dev.edge = INT_NONE;
	//32 pins per bank, in order a b c ...
	dev.gpionum = (bank - 'A')*32 + pin;
	std::stringstream ss;
	ss << "pio" << bank << pin << "/";
	dev.gpioref = ss.str();
	return BusManager<GPIODevice>::attachDevice(dev);
}

/*! Parses the inputs into those needed by initialize and the other methods to operate on the gpio
 *  For GPIOs that will be edge triggered interrupts
 * \param bank The bank of the gpio. Can be A, B, C, D, or E
 * \param pin The pin in the bank to use. Can be between 0 and 31
 * \param edge The edge to trigger on Rising/falling
 * \return the id of the registered device
 */
int GPIOManager::attachDevice(char bank, int pin, GPIOEdge type){
	assert(bank >= 'A');
	assert(bank <= 'E');
	assert(pin >= 0);
	assert(pin <= 31);
	GPIODevice dev;
	dev.direction = GPIO_INPUT;
	dev.edge = type;

	//32 pins per bank, in order a b c ...
	dev.gpionum = (bank - 'A')*32 + pin;

	//gpios will create a folder pioXYY where X is the bank and YY is the pin
	std::stringstream ss;
	ss << "pio" << bank << pin << "/";
	dev.gpioref = ss.str();
	return BusManager<GPIODevice>::attachDevice(dev);
}

/*! Poll until an interrupt is received or we timeout
 * \param id the id of the device to poll on
 * \param timeout the number of milliseconds to wait before timeing out
 * \return true if an interrupt was received
 */
bool GPIOManager::wait(int id, uint32_t timeout){
	GPIODevice& dev = getDevice(id);
	LockGuard(dev.lock);

	int retval = poll(&dev.pfd, 1, timeout);
	if (!(dev.pfd.revents & POLLPRI) || retval == 0) {
		//Timeout
		Logger::Stream(LEVEL_WARN, tags) << "Interrupt timeout occured on " << dev.gpioref;
		return false;
	}

	Logger::Stream(LEVEL_DEBUG, tags) << "Interrupt received on " << dev.gpioref;

	//Clear any pending interrupts
	uint8_t dummy;
	read(dev.pfd.fd, &dummy, 1);
	return true;
}


/*! Determines if the state of an input is high or low
 * \param id the id of the device to read
 * \return the level of the GPIO
 */
GPIOLevel GPIOManager::get(int id){
	GPIODevice& dev = getDevice(id);
	LockGuard(dev.lock);
	assert(dev.direction == GPIO_INPUT);
	assert(dev.edge == INT_NONE);

	std::ifstream fs;
	int val;
	fs.open((gpiobase + dev.gpioref + "value").c_str());
	fs >> val;
	fs.close();

	Logger::Stream(LEVEL_DEBUG, tags) << "Read " << ((val == 1) ? "HIGH" : "LOW")<< " from " << dev.gpioref;
	return (val == 1) ? HIGH : LOW;
}

/*! Sets the provided gpio to the specified value
 * \param id the id of the gpio to set
 * \param level whether to write high or low to the gpio
 */
void GPIOManager::set(int id, GPIOLevel level){
	GPIODevice& dev = getDevice(id);
	LockGuard(dev.lock);
	assert(dev.direction == GPIO_OUTPUT);
	assert(dev.edge == INT_NONE);

	Logger::Stream(LEVEL_DEBUG, tags) << "Setting " << dev.gpioref << " to " << ((level == HIGH)?"HIGH":"LOW");

	std::ofstream fs;
	int val = (level == HIGH) ? 1 : 0;
	fs.open((gpiobase + dev.gpioref + "value").c_str());
	fs << val;
	fs.close();
}
