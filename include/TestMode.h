///*
// * TestMode.h
// *
// *  Created on: Feb 20, 2011
// *      Author: Riley
// */
//
//#ifndef TESTMODE_H_
//#define TESTMODE_H_
//
//#include "core/SystemMode.h"
//
//#define TEST_MODE_NUM_INSTANCES 2
//
//namespace Phoenix
//{
//    namespace Core
//    {
//        class TestMode : public Phoenix::Core::SystemMode
//        {
//        public:
//            static bool Initialize(void);
//            static TestMode * GetInstance(size_t num);
//
//            void OnEntry(void);
//            void OnExit(void);
//
//            virtual ~TestMode( );
//
//            bool CheckOnEntry(void);
//            bool CheckOnExit(void);
//
//            static void DestroySubclass(void);
//
//        private:
//            static TestMode instances[TEST_MODE_NUM_INSTANCES];
//            bool onEntryCalled;
//            bool onExitCalled;
//
//            TestMode(std::size_t num = 0);
//        };
//    }
//}
//
//#endif /* TESTMODE_H_ */
