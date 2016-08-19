/* testdisp0011.cpp
 * Created on: Jul 7, 2014
 *    Author: fsw
 *  \brief Test 11 of the Dispatcher Class.
 *
 *  This test verifies that the when a Dispatched packet is received by the
 *  destination server but the handler hangs, Wait correctly returns
 *  MSG_RCVD_NO_RESP_SENT.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

class Test0011MessageHandler : public MessageHandler
{
public:
    ReturnMessage * Handle(const ACPPacket & packet);
}testHandler11;

ReturnMessage * Test0011MessageHandler::Handle(const ACPPacket & packet)
{
    if (packet.GetMessagePtr()->GetOpcode() == 0)
    {
		cout << "TestMessageHandle: Received opcode 0" << endl;
        while (1)
        {
            usleep(10000);
        }
    }
    else
    {
		cout << "TestMessageHandle: Received opcode !0" << endl;
        CommandMessage msg(packet.GetMessagePtr()->GetOpcode()+10);
        ReturnMessage *ret = new ReturnMessage(&msg, true);
        return ret;
    }
}

static sem_t sync0;

static void * taskTest1(void * parameters)
{
    {
        Dispatcher * dispatcher;
        ACPPacket * packet1, * packet2;
        ReturnMessage retMsg;
        size_t i = 0;
        DispatcherStatusEnum status;

        dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

        if (NULL == dispatcher)
        {
            ADD_FAILURE(); pthread_exit(0);
        }

        cout << "Initialized" << endl;

        CommandMessage * cmd = new CommandMessage(0);

        packet1 = new ACPPacket(1, 2, 0, 0, cmd);
        if (!dispatcher->Dispatch(*packet1))
        {
            ADD_FAILURE(); pthread_exit(0);
        }
        delete cmd;

        cmd = new CommandMessage(1);
        packet2 = new ACPPacket(1, 2, 1, 0, cmd);
        if (!dispatcher->Dispatch(*packet2))
        {
            ADD_FAILURE(); pthread_exit(0);
        }
        delete cmd;

        if (DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT != (status = dispatcher->WaitForDispatchResponse(*packet1, retMsg)))
        {
            cout << "Status: " << status << endl;
            ADD_FAILURE(); pthread_exit(0);
        }

        cout << "Done Waiting" << endl;

        uint64_t LastWakeTime;
        for (i = 0; i < 10; ++i)
        {
			LastWakeTime = getTimeInMilis();
            if (DISPATCHER_STATUS_OK == (status = dispatcher->WaitForDispatchResponse(*packet1, retMsg)))
            {
                break;
            }
            cout << "Waiting " << i << endl;
			cout << "Last tick time " << LastWakeTime << endl;
        }

        if (i == 10)
        {
            cout << "Status: " << status << endl;
            ADD_FAILURE(); pthread_exit(0);
        }

        if (retMsg.GetSuccess() ||
                retMsg.GetOpcode() != ERROR_OPCODE_HANDLER_TIMED_OUT ||
                retMsg.GetType() != MESSAGE_TYPE_ERROR)
        {
            cout << "Success: " << retMsg.GetSuccess()
				         << "  Opcode: " << retMsg.GetOpcode()
				         << "  Type: " << retMsg.GetType() << endl;
            ADD_FAILURE(); pthread_exit(0);
        }

        xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0);

        if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet2, retMsg)))
        {
            cout << "Status: " << status << endl;
            ADD_FAILURE(); pthread_exit(0);
        }

        if (!retMsg.GetSuccess() ||
                retMsg.GetOpcode() != 11 ||
                retMsg.GetType() != MESSAGE_TYPE_COMMAND)
        {
            cout << "Success: " << retMsg.GetSuccess()
				         << "  Opcode: " << retMsg.GetOpcode()
				         << "  Type: " << retMsg.GetType() << endl;
            ADD_FAILURE(); pthread_exit(0);
        }

        delete packet1;
        delete packet2;
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
            ADD_FAILURE(); pthread_exit(0);
        }

        cout << "Initialized" << endl;
        reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler11);
        reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), &testHandler11);
        arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), true);
        arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), true);
        ret = dispatcher->AddRegistry(2, &reg, &arby);
        if (!ret)
        {
            ADD_FAILURE(); pthread_exit(0);
        }

        cout << "Registry Added" << endl;

        for (i = 0; i < 10; ++i)
        {
        	cout << "litening with i: " << i << endl;
            if (dispatcher->Listen(2))
            {
                break;
            }
            usleep(100000);
        }

        cout << "Done Listening" << endl;

        if (i == 10)
        {
            ADD_FAILURE(); pthread_exit(0);
        }
    }

    xSemaphoreGive(&sync0);

    pthread_exit(0);
}


TEST(TestDispatcher, MSG_RCVD_NO_RESP_SENT)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync0));

	ASSERT_FALSE(true != xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);


	sem_destroy(&sync0);
}
