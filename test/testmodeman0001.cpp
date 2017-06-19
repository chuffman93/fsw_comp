/* testmodeman0001.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *  \brief Test 1 of the ModeManager Class.
 *
 *  This test verifies that the GetInstance method of the ModeManager class
 *  correctly returns the same reference to a ModeManager object on subsequent
 *  calls to the method.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Factory.h"



#include "core/ModeManager.h"

using namespace std;
using namespace AllStar::Core;

static void * taskTest(void * parameters)
{
    ModeManager * manager, *manager2;

    manager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));;

    cout << "Got instance 1" << endl;

    if (NULL == manager)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    manager2 = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));;

    cout << "Got instance 2" << endl;

    if (manager != manager2)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    pthread_exit(0);
}


TEST(TestModeManager, GetInstance)
{
	cout << "Start of Test" << endl;
    cout << "ModeManager Initialized" << endl;

    pthread_t testThread;
    pthread_create(&testThread, NULL, &taskTest, NULL);

    pthread_join(testThread, NULL);
    Factory::Destroy(MODE_MANAGER_SINGLETON);
}
