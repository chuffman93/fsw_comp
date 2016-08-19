/* testdisp0003.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 3 of the Dispatcher Class.
 *
 *  This test verifies that the Dispatch method correctly sends messages to
 *  another location, the Listen method is able to receive the message,
 *  and WaitForDispatchResponse correctly identifies a response to the
 *  initial message.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;


class TestMessageHandler : public MessageHandler
{
public:
    ReturnMessage * Handle(const ACPPacket & packet);
} testHandler3;

ReturnMessage * TestMessageHandler::Handle(const ACPPacket & packet)
{
    CommandMessage msg(0);
    ReturnMessage * ret = new ReturnMessage(&msg, true);
    return ret;
}

static void * taskTest1(void * parameters)
{
    {
        Dispatcher * dispatcher;
        ACPPacket * packet;
        ReturnMessage retMsg;
        DispatcherStatusEnum status;

        dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

        if (NULL == dispatcher)
        {
            ADD_FAILURE() << "dispatcher NULL";
            pthread_exit(0);
        }


        CommandMessage * cmd = new CommandMessage(0);
        packet = new ACPPacket(1, 2, 0, 0, cmd);
        delete cmd;

        if (!dispatcher->Dispatch(*packet))
        {
            ADD_FAILURE() << "dispatch packet failed";
            pthread_exit(0);
        }

        cout << "packet dispatched" << endl;
        if (DISPATCHER_STATUS_OK != (status
                = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
        {
            ADD_FAILURE() << "Return Value: " << status;
            pthread_exit(0);
        }

        if (!retMsg.GetSuccess( ) || retMsg.GetOpcode( ) != 0
                || retMsg.GetType( ) != MESSAGE_TYPE_COMMAND)
        {

           ADD_FAILURE() << "Success: " << retMsg.GetSuccess( ) << "  Opcode: "
                   << (uint8) retMsg.GetOpcode( ) << "  Type: "
                   << retMsg.GetType( );

           pthread_exit(0);
        }

        delete packet;
    }
    pthread_exit(0);

}

static void * taskTest2(void * parameters)
{
    {

        Dispatcher * dispatcher;
        MessageHandlerRegistry reg;
        Arbitrator arby(2);
        bool ret;
        size_t i;

        dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

        if (NULL == dispatcher)
        {
            ADD_FAILURE() << "dispatcher NULL";
            pthread_exit(0);
        }

        reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0),
                &testHandler3);
        arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0),
                true);
        ret = dispatcher->AddRegistry(2, &reg, &arby);
        if (!ret)
        {
            ADD_FAILURE() << "ret failure";
            pthread_exit(0);
        }

        for (i = 0; i < 10; ++i)
        {
            if (dispatcher->Listen(2))
            {
                break;
            }
            //usleep(1000000);
            sleep(1);


        }

        if (i == 10)
        {
            ADD_FAILURE() << "couldn't find message";
            pthread_exit(0);
        }

    }

   pthread_exit(0);
}


TEST(TestDispatcher, SendListenWait)
{
		mq_unlink("/subsystemQueueHandle");
		mq_unlink("/queueHandle");

		Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		pthread_t testThread1;
		pthread_t testThread2;

		pthread_create(&testThread1, NULL, &taskTest1, NULL);
		pthread_create(&testThread2, NULL, &taskTest2, NULL);

		pthread_join(testThread1, NULL);
		pthread_join(testThread2, NULL);

		Factory::Destroy(DISPATCHER_SINGLETON);
}
