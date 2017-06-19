/*! \file testmodeman0005.cpp
 * Created on: Jul 9, 2014
 *    Author: fsw
 *  \brief Test 5 of the ModeManager Class.
 *
 *  This test verifies that the SetMode method of the ModeManager class
 *  returns false if the current mode is passed in as the new mode.  It also
 *  tests that SetMode correctly calls OnEntry for the new mode even when
 *  the current mode is NULL and that SetMode calls both OnExit and OnEntry
 *  if the current mode is not NULL, but only if the transition is allowed.
 *  Finally, the test ensures that UpdateAll is called for each listener
 *  that is Attached to the Manager, and does not call update for those that have been Detached.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
#include "core/Factory.h"

#include "TestMode.h"

using namespace std;
using namespace AllStar::Core;

class TestListener0005 : public ModeListener
{
public:
    virtual void Update(const SystemMode * mode)
    {
        ++updateCounter;
    }
    ;

    void ResetUpdateCount(void)
    {
        updateCounter = 0;
    }

    static size_t GetUpdateCount(void)
    {
        return updateCounter;
    }

private:
    static size_t updateCounter;
};

size_t TestListener0005::updateCounter = 0;

static void* taskTest(void * parameters)
{
    ModeManager * manager;
    TestMode * testMode, *testMode2;
    TestListener0005 tlist1, tlist2;

    manager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
    testMode = TestMode::GetInstance(0);
    testMode2 = TestMode::GetInstance(1);

    if (NULL == testMode || NULL == testMode2 || testMode == testMode2)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (NULL == manager)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (!manager->Attach(tlist1))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (!manager->Attach(tlist2))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (tlist1.GetUpdateCount( ) != 0)
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Initialized" << endl;

    if (NULL != manager->GetMode( ))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Got NULL Mode" << endl;

    if (!manager->SetMode(MODE_TEST, 0))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << manager->GetMode()->GetID() << endl;

    if (testMode != manager->GetMode( ) || !testMode->CheckOnEntry( )
            || testMode->CheckOnExit( ))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (tlist1.GetUpdateCount( ) != 2)
    {
        cout << "Count: " << tlist1.GetUpdateCount( ) << endl;
        ADD_FAILURE(); pthread_exit(0);
    }

    tlist1.ResetUpdateCount( );

    if (tlist1.GetUpdateCount( ) != 0)
    {
        cout << "Count: " << tlist1.GetUpdateCount( ) << endl;
        ADD_FAILURE(); pthread_exit(0);
    }

    if (!manager->Detach(tlist2))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    cout << "Set Mode" << endl;

    if (manager->SetMode(MODE_TEST, 0))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (testMode != manager->GetMode( ) || testMode->CheckOnEntry( )
            || testMode->CheckOnExit( ))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (tlist1.GetUpdateCount( ) != 0)
    {
        cout << "Count: " << tlist1.GetUpdateCount( ) << endl;
        ADD_FAILURE(); pthread_exit(0);
    }

    tlist1.ResetUpdateCount( );

    cout << "Set Same Mode" << endl;

    if (!manager->SetMode(MODE_TEST2, 0))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (testMode2 != manager->GetMode( ) || testMode->CheckOnEntry( )
            || !testMode->CheckOnExit( ) || !testMode2->CheckOnEntry( )
            || testMode->CheckOnExit( ))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (tlist1.GetUpdateCount( ) != 1)
    {
        cout << "Count: " << tlist1.GetUpdateCount( ) << endl;
        ADD_FAILURE(); pthread_exit(0);
    }

    tlist1.ResetUpdateCount( );

    if (manager->SetMode(MODE_TEST2, 0))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (testMode2 != manager->GetMode( ) || testMode->CheckOnEntry( )
            || testMode->CheckOnExit( ) || testMode2->CheckOnEntry( )
            || testMode->CheckOnExit( ))
    {
        ADD_FAILURE(); pthread_exit(0);
    }

    if (tlist1.GetUpdateCount( ) != 0)
    {
        cout << "Count: " << tlist1.GetUpdateCount( ) << endl;
        ADD_FAILURE(); pthread_exit(0);
    }

    tlist1.ResetUpdateCount( );

    pthread_exit(0);
}

TEST(TestModeManager, SetMode)
{
	pthread_t testThread;
	pthread_create(&testThread, NULL, &taskTest, NULL);


	pthread_join(testThread, NULL);

	Factory::Destroy(MODE_MANAGER_SINGLETON);
}
