/* testmodeman0002.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *  \brief Test 2 of the ModeManager Class.
 *
 *  This test verifies that the Attach method of the ModeManager class
 *  only allows a given ModeListener to be inserted once.
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

    cout << "Added First Item" << endl;

    ret = manager->Attach(listener);
    cout << "Added Second Item" << endl;
    if (ret)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    pthread_exit(0);

}


TEST(TestModeManager, Attach)
{
	pthread_t testThread;
	pthread_create(&testThread, NULL, &taskTest, NULL);
	pthread_join(testThread, NULL);
	Factory::Destroy(MODE_MANAGER_SINGLETON);
}
