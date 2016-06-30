/*
 * ModeManager.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: Riley
 */

#include "core/ModeManager.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/ComMode.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
#include "TestMode.h"
#endif

#define MODE_DEBUG			1

#if MODE_DEBUG
#include <iostream>
#define DEBUG_MODE_COUT(m) std::cout << m << std::endl;
#else
#define DEBUG_MODE_COUT(m)
#endif // DEBUG

using namespace std;

namespace Phoenix
{
    namespace Core
    {
        SystemMode * ModeManager::modeList[MODE_NUM_MODES];

		ModeManager::ModeManager(void) 
				: Singleton(), mode(NULL) 
		{
			
		}

        ModeManager::~ModeManager(void)
        {
			
        }
		
		void ModeManager::Initialize(void)
		{
			SystemMode::Initialize();
			
			modeList[MODE_ACCESS] = AccessMode::GetInstance();
			modeList[MODE_STARTUP] = StartupMode::GetInstance();
			modeList[MODE_BUS_PRIORITY] = BusPriorityMode::GetInstance();
			modeList[MODE_PLD_PRIORITY] = PayloadPriorityMode::GetInstance();
			modeList[MODE_ERROR] = ErrorMode::GetInstance();
			modeList[MODE_COM] = ComMode::GetInstance();
#ifdef TEST
			modeList[MODE_TEST] = TestMode::GetInstance(0);
			modeList[MODE_TEST2] = TestMode::GetInstance(1);
#endif

		}
		
		bool ModeManager::IsFullyInitialized(void)
		{
			bool isInit = Singleton::IsFullyInitialized();
			for(int i = 0; i < MODE_NUM_MODES; i++)
			{

				isInit &= (modeList[i] != NULL);
			}
			return isInit;
		}
		
#ifdef TEST
		void ModeManager::Destroy(void)
		{
			//SystemMode::Destroy();
		}
#endif

        bool ModeManager::Attach(ModeListener & listener)
        {
            if (true == this->TakeLock(MAX_BLOCK_TIME))
            {
                pair<set<ModeListener *>::const_iterator,bool> result = listeners.insert(&listener);
                this->GiveLock();
                return result.second;
            }
            else
            {
                return false;
            }
        }

        bool ModeManager::Detach(ModeListener & listener)
        {
            bool ret;

            if (true == this->TakeLock(MAX_BLOCK_TIME))
            {
                ret = (listeners.erase(&listener) > 0);
                this->GiveLock();
                return ret;
            }
            else
            {
                return false;
            }
        }

        const SystemMode * ModeManager::GetMode(void)
        {
            return mode;
        }

        bool ModeManager::SetMode(SystemModeEnum newMode, LocationIDType server)
        {
        	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("SetMode called", LOGGER_LEVEL_DEBUG);
#ifdef HARDWARE
			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
#endif //HARDWARE

			//Mode is null jackass
			//SystemModeEnum prevMode = this->mode->GetID();
			logger->Log("----SetMode: Before take lock", LOGGER_LEVEL_DEBUG);
            if (true == this->TakeLock(MAX_BLOCK_TIME))
            {
				logger->Log("----SetMode: Checking for NULL mode", LOGGER_LEVEL_DEBUG);
                if (NULL != this->mode)
                {
					logger->Log("----SetMode: Current Mode is not NULL", LOGGER_LEVEL_DEBUG);
                    if (newMode == this->mode->GetID())
                    {
                        this->GiveLock();
                        return false;
                    }

                    if (!this->mode->CheckValidTransition(newMode, server))
                    {
                        this->GiveLock();
                        return false;
                    }

                    this->mode->OnExit( );
                }
				logger->Log("----SetMode: Current Mode is NULL", LOGGER_LEVEL_DEBUG);

                if (newMode >= MODE_FIRST_MODE && newMode < MODE_NUM_MODES)
				{
					this->mode = modeList[newMode];
				}
				else
				{
					logger->Log("----SetMode: Invalid mode", LOGGER_LEVEL_ERROR);
					this->GiveLock();
					return false;
				}
				
				logger->Log("--------SetMode: Calling OnEntry and Notify all", LOGGER_LEVEL_DEBUG);
                this->mode->OnEntry();

                NotifyAll();
				// TODO: need mode logger here
#ifdef HARDWARE
				fileHandler->logAppend(LOG_MODE, prevMode, newMode);
#endif //HARDWARE
				
                this->GiveLock();
				logger->Log("------------SetMode: success", LOGGER_LEVEL_DEBUG);
                return true;
            }
            else
            {
                return false;
            }
        }

        void ModeManager::NotifyAll(void)
        {
            for (set<ModeListener *>::iterator i = listeners.begin(); i != listeners.end(); ++i)
            {
                (*i)->Update(this->mode);
            }
        }
    }
}
