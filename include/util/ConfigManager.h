#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <string>
#include "core/Factory.h"
#include "sys/mman.h"
#include "Logger.h"
namespace AllStar{
namespace util{

template <class ConfigStruct> class ConfigManager{
	friend class AllStar::Core::Factory;
public:
	ConfigManager(std::string fileName)
		:fileName(fileName){}
	ConfigStruct config;

	void LoadConfig(){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		int fd;
		fd = open((".config/" + fileName).c_str(), O_RDWR);
		ConfigStruct * configuration;
		if(fd != -1){
			configuration = (ConfigStruct*)mmap(0, sizeof(ConfigStruct), PROT_READ, MAP_SHARED, fd, 0);

			if(configuration != NULL && configuration != (void*)-1){
				memcpy(&config, configuration, sizeof(ConfigStruct));
			}else
				logger->Log(LOGGER_LEVEL_INFO, "CONFIG Manager Mmap fail");

			munmap(configuration, sizeof(ConfigStruct));
			close(fd);
		}else{
			logger->Log(LOGGER_LEVEL_INFO, "CONFIG Manager bad file");
		}
	}
private:
	std::string fileName;

};


}
}

#endif
