/* testdisp0004.cpp
 * Created on: Jun 12, 2014
 *    Author: fsw
 *  \brief Test 4 of the Dispatcher Class.
 *
 *  This test verifies that if two servers are both trying to send messages
 *  to each other at the same time, each is able to successfully the
 *  message.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/CommandMessage.h"
using namespace std;
using namespace AllStar::Core;

class TestMessageHandler : public MessageHandler
{
public:
    ReturnMessage * Handle(const ACPPacket & packet);
} testHandler4;

//ReturnMessage * TestMessageHandler::Handle(const ACPPacket & packet)
//{
//    CommandMessage msg(0);
//    ReturnMessage * ret = new ReturnMessage(&msg, true);
//    return ret;
//}

static sem_t sync1, sync2;

static void * taskTest1(void * parameters)
{
	{
		Dispatcher * dispatcher;
		MessageHandlerRegistry reg;
		Arbitrator arby(1);
		ACPPacket * packet;
		ReturnMessage retMsg;
		size_t i;
		bool ret;
		DispatcherStatusEnum status;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Task 1 Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler4);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), true);
		ret = dispatcher->AddRegistry(1, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		CommandMessage * cmd = new CommandMessage(0);
		packet = new ACPPacket(1, 2, 0, 0, cmd);
		delete cmd;

		cout << "Task 1 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync2);

		if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}



		cout << "Task 1 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}


		cout << "Task 1 Listening" << endl;
		for (i = 0; i < 10; ++i)
		{
			if (dispatcher->Listen(1))
			{
				break;
			}
			usleep(1000000);
		}

		cout << "Task 1 Listen Done" << endl;
		if (i == 10)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Task 1 Waiting for Response" << endl;
		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Task 1 Response Received" << endl;
		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 0 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE();
			pthread_exit(0);
		}

		xSemaphoreTake(&sync1);

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
		ACPPacket * packet;
		ReturnMessage retMsg;
		size_t i;
		bool ret;
		DispatcherStatusEnum status;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Task 2 Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler4);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), true);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		CommandMessage * cmd = new CommandMessage(0);
		packet = new ACPPacket(2, 1, 0, 0, cmd);
		delete cmd;

		cout << "Task 2 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync1);
		usleep(1000);
		if ((true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Task 2 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}



		cout << "Task 2 Listening" << endl;
		for (i = 0; i < 10; ++i)
		{
			if (dispatcher->Listen(2))
			{
				break;
			}
			usleep(1000000);
		}

		cout << "Task 2 Listen Done" << endl;
		if (i == 10)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Task 2 Waiting for Response" << endl;
		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE();
			pthread_exit(0);
		}


		cout << "Task 2 Response Received" << endl;
		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 0 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE();
			pthread_exit(0);
		}

		delete packet;
	}

	xSemaphoreGive(&sync1);

	pthread_exit(0);
}


TEST(TestDispatcher, DualSend)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	ASSERT_TRUE(vSemaphoreCreateBinary(&sync1) == true);
	ASSERT_TRUE(vSemaphoreCreateBinary(&sync2) == true);

	if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)) ||
		(true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0)))
	{
		ADD_FAILURE() << "Cannot Take Semaphore!";
		pthread_exit(0);
	}

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);

	sem_destroy(&sync1);
	sem_destroy(&sync2);
}
