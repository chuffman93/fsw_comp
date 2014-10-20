/*
 * TestMode.cpp
 *
 *  Created on: Feb 20, 2011
 *      Author: Riley
 */

#include "TestMode.h"
#include <iostream>
using namespace std;

namespace Phoenix
{
    namespace Core
    {
        TestMode TestMode::instances[TEST_MODE_NUM_INSTANCES];

        bool TestMode::Initialize(void)
        {
            for (size_t i = 0; i < TEST_MODE_NUM_INSTANCES; ++i)
            {
                instances[i] = TestMode(i);
            }

            return true;
        }

        TestMode * TestMode::GetInstance(size_t num)
        {
            return ((num < TEST_MODE_NUM_INSTANCES) ? &(instances[num]) : NULL);
        }

        void TestMode::OnEntry(void)
        {
            onEntryCalled = true;
            cout << "TestMode::OnEntry Called: ID = " << GetID( ) << endl;
        }

        void TestMode::OnExit(void)
        {
            onExitCalled = true;
            cout << "TestMode::OnExit Called: ID = " << GetID( ) << endl;
        }

        bool TestMode::CheckOnEntry(void)
        {
            bool ret = onEntryCalled;
            onEntryCalled = false;
            return ret;
        }
        bool TestMode::CheckOnExit(void)
        {
            bool ret = onExitCalled;
            onExitCalled = false;
            return ret;
        }

        TestMode::~TestMode( )
        {

        }

        void TestMode::DestroySubclass()
        {

        }

        TestMode::TestMode(size_t num) :
                SystemMode(MODE_TEST), onEntryCalled(false), onExitCalled(false)
        {
            systemMode = (SystemModeEnum)min(size_t(MODE_TEST + num), (size_t)(MODE_NUM_MODES - 1));
            transitionTable[MODE_TEST] = Permission(false);
            transitionTable[MODE_TEST2] = Permission(true);
        }
    }
}
