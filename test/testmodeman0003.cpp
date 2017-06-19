/* testmodeman0003.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *  \brief Test 3 of the ModeManager Class.
 *
 *  This test verifies that the Detach method of the ModeManager class
 *  does not allow the same ModeListener object to be Detached twice.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Factory.h"

#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

class TestListener : public ModeListener
{
public:
    virtual void Update(const SystemMode * mode) { };
};

static void * taskTest(void * parameters)
{
    ModeManager * manager;
    TestListener listener;
    bool ret;

    manager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

    if (NULL == manager)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Initialized" << endl;

    ret = manager->Attach(listener);
    if (!ret)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Added Item" << endl;

    ret = manager->Detach(listener);
    cout << "Removed Item" << endl;
    if (!ret)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    ret = manager->Detach(listener);
    cout << "Removed Item Again" << endl;
    if (ret)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    pthread_exit(0);
}


TEST(TestModeManager, Detach)
{
	pthread_t testThread;
	pthread_create(&testThread, NULL, &taskTest, NULL);
	pthread_join(testThread, NULL);

    Factory::Destroy(MODE_MANAGER_SINGLETON);
}
