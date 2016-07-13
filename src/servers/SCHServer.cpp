
#include "servers/SCHServer.h"

#include "servers/GPSServer.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/PayloadPriorityMode.h"
#include "core/SystemMode.h"
#include "util/Logger.h"
#include <sys/mman.h>
#include <stdio.h>

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		
		SCHServer::SCHServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			scheduleRunning = false;
			//TODO Setup default schedule
		}

		SCHServer::~SCHServer()
		{
			// Left Intentionally Blank
		}

		SCHServer & SCHServer::operator=(const SCHServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

 			return *this;
		}
		
		void SCHServer::Initialize(void)
		{
		}
		
#ifdef TEST
		void SCHServer::Destroy(void)
		{
		}
#endif
		
		bool SCHServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void SCHServer::Update(const SystemMode * mode)
		{
			//update mode stuff!
		}
		
		bool SCHServer::RegisterHandlers()
		{
			volatile bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}
		
		void SCHServer::SubsystemLoop(void)
		{
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			uint64_t LastTimeSwitched = getTimeInMillis();
			currentSchedule = std::list<SCHItem>();

			ReadConfig();
			while(1)
			{
				uint64_t LastWakeTime = getTimeInMillis();
				if(currentSchedule.empty()){
					logger->Log("Fetching Next Schedule", LOGGER_LEVEL_INFO);
					LoadNextSchedule();
					LastTimeSwitched = getTimeInMillis();
					modeManager->SetMode(currentSchedule.front().mode, LOCATION_ID_INVALID);
				}

				GPSServer * gpsServer = dynamic_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
				SCHItem CurrentEvent = currentSchedule.front();
				bool inRange = gpsServer->DistanceTo(CurrentEvent.latitude, CurrentEvent.longitude) < CurrentEvent.radius;
				bool timeout = CurrentEvent.timeoutms < (getTimeInMillis() - LastTimeSwitched);
				if(inRange || timeout){
					currentSchedule.pop_front();
					if(currentSchedule.empty()){
						modeManager->SetMode(MODE_BUS_PRIORITY, LOCATION_ID_INVALID);
					}else{
						modeManager->SetMode(currentSchedule.front().mode, LOCATION_ID_INVALID);
					}
					LastTimeSwitched = getTimeInMillis();
				}
				waitUntil(LastWakeTime, 1000);
			}
		}

		void SCHServer::LoadNextSchedule(){
			bool failure = false;
			currentSchedule = std::list<SCHItem>();
			if(this->TakeLock(MAX_BLOCK_TIME)){
				if(access(SCH_SCHEDULE_FILE, F_OK) != -1){
					FILE *pFile;
					pFile = fopen(SCH_SCHEDULE_FILE, "r");
					if(pFile != NULL){
						for(int i = 0; i < SCHEDULE_MAX_SIZE; i++){
							SCHItem tmp;
							if(fgets((char*) &tmp, (int) sizeof(tmp), pFile) != NULL){
								if(tmp.mode >= MODE_FIRST_MODE && tmp.mode <= MODE_NUM_MODES)
									currentSchedule.push_back(tmp);
								else{
									failure = true;
									break;
								}
							}else{
								failure = true;
								break;
							}
						}
						fclose(pFile);
						remove(SCH_SCHEDULE_FILE);
					}else{
						failure = true;
					}
				}else{
					failure = true;
				}
				this->GiveLock();
			}else{
				failure = true;
			}
			if(failure){
				currentSchedule = defaultSchedule;
			}
		}
		
		void SCHServer::ReadConfig(){
			int fd;
			SCHConfig * configuration;
			if(true == this->TakeLock(MAX_BLOCK_TIME)){
				fd = open(SCH_CONFIG_FILE, O_RDWR);

				if(fd != -1){
					configuration = (SCHConfig*)mmap(0, sizeof(SCHConfig), PROT_READ, MAP_SHARED, fd, 0);
					if(configuration != NULL && configuration != (void*)-1){
						printf("Number of items: %d\n",configuration->sizeOfDefaultSchedule);
						for(int i = 0; i < configuration->sizeOfDefaultSchedule; i++){
							printf("Added item to default schedule: {%f, %f, %f, %d, %d}\n",
									configuration->defaultScheduleArray[i].latitude,
									configuration->defaultScheduleArray[i].longitude,
									configuration->defaultScheduleArray[i].radius,
									configuration->defaultScheduleArray[i].timeoutms,
									configuration->defaultScheduleArray[i].mode);
							defaultSchedule.push_back(configuration->defaultScheduleArray[i]);
						}
					}else
						perror("MMap failure");
					munmap(configuration, sizeof(SCHConfig));
					close(fd);
				}else{
					perror("FD error");
					printf("path is %s\n", SCH_CONFIG_FILE);
				}
				this->GiveLock();
			}
		}
	}
}
