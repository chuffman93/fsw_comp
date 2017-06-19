/* testmodeman0004.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *  \brief Test 4 of the ModeManager Class.
 *
 *  This test verifies that the GetMode method of the ModeManager class
 *  returns NULL until a mode has explicitly been set.  Once the mode has
 *  been set, the returned mode should match the global instance of the mode.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ModeManager.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

static void* taskTest(void * parameters)
{
    ModeManager * manager;
    bool ret;

    manager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

    if (NULL == manager)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Initialized" << endl;

    if (NULL != manager->GetMode())
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Got NULL Mode" << endl;

    if (!manager->SetMode(MODE_TEST, 0))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Set Mode" << endl;

    if (MODE_TEST != manager->GetMode()->GetID( ))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Got New Mode" << endl;

    pthread_exit(0);

}

TEST(TestModeManager, GetMode)
{
	pthread_t testThread;
	pthread_create(&testThread, NULL, &taskTest, NULL);


	pthread_join(testThread, NULL);

    Factory::Destroy(MODE_MANAGER_SINGLETON);

}
