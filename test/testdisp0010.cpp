/* testdisp0010.cpp
 * Created on: Jul 7, 2014
 *    Author: fsw
 *  \brief Test 10 of the Dispatcher Class.
 *
 *  This test verifies that if three servers are defined in the system and that
 *  two of the servers are trying to send a command to the other while that
 *  server is sending a command to each of the other servers, all messages
 *  are routed correctly.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
using namespace std;


#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"
using namespace std;
using namespace AllStar::Core;

class Test0010MessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler10;

ReturnMessage * Test0010MessageHandler::Handle(const ACPPacket & packet)
{
	CommandMessage msg(packet.GetMessagePtr()->GetOpcode()+10);
	ReturnMessage *ret = new ReturnMessage(&msg, true);
	return ret;
}

static sem_t sync1, sync2, sync3, done2, done3;

// This server sends to each of the others
static void * taskTest1(void * parameters)
{
	{
		Dispatcher * dispatcher;
		MessageHandlerRegistry reg;
		Arbitrator arby(1);
		ACPPacket * packet1, * packet2;
		ReturnMessage retMsg;
		size_t i;
		bool ret;
		DispatcherStatusEnum status;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), &testHandler10);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), true);
		ret = dispatcher->AddRegistry(1, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		CommandMessage * cmd = new CommandMessage(2);
		packet1 = new ACPPacket(1, 2, 0, 0, cmd);
		delete cmd;
		cmd = new CommandMessage(3);
		packet2 = new ACPPacket(1, 3, 1, 0, cmd);
		delete cmd;

		cout << "Task 1 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync2);
		if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet1))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet2))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Listening" << endl;
		for (i = 0; i < 10; ++i)
		{
			if (dispatcher->Listen(1))
			{
				break;
			}
			usleep(100000);
		}

		cout << "Task 1 Listen Done" << endl;
		if (i == 10)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Listening" << endl;
		for (i = 0; i < 10; ++i)
		{
			if (dispatcher->Listen(1))
			{
				break;
			}
			usleep(100000);
		}

		cout << "Task 1 Listen Done" << endl;
		if (i == 10)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Waiting for Response" << endl;
		for (i = 0; i < 10; ++i)
		{
			if (DISPATCHER_STATUS_OK == (status = dispatcher->WaitForDispatchResponse(*packet1, retMsg)))
			{
				break;
			}
		}

		if (i == 10)
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Response Received" << endl;
		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 12 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		for (i = 0; i < 10; ++i)
		{
			if (DISPATCHER_STATUS_OK == (status = dispatcher->WaitForDispatchResponse(*packet2, retMsg)))
			{
				break;
			}
		}

		if (i == 10)
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Response Received" << endl;
		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 13 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if ((true != xSemaphoreTake(&done2, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		if ((true != xSemaphoreTake(&done3, MAX_BLOCK_TIME, 0)))
		{
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
		ACPPacket * packet;
		ReturnMessage retMsg;
		size_t i;
		bool ret;
		DispatcherStatusEnum status;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 2 Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 2), &testHandler10);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 2), true);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		CommandMessage * cmd = new CommandMessage(1);
		packet = new ACPPacket(2, 1, 2, 0, cmd);
		delete cmd;

		cout << "Task 2 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync3);
		if ((true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 2 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 2 Listening" << endl;
		for (i = 0; i < 10; ++i)
		{
			if (dispatcher->Listen(2))
			{
				break;
			}
			usleep(100000);
		}

		cout << "Task 2 Listen Done" << endl;
		if (i == 10)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 2 Waiting for Response" << endl;
		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 2 Response Received" << endl;
		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 11 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		delete packet;
		xSemaphoreGive(&done2);
	}

	pthread_exit(0);
}

static void * taskTest3(void * parameters)
{
	{
		Dispatcher * dispatcher;
		MessageHandlerRegistry reg;
		Arbitrator arby(3);
		ACPPacket * packet;
		ReturnMessage retMsg;
		size_t i;
		bool ret;
		DispatcherStatusEnum status;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 3 Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 3), &testHandler10);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 3), true);
		ret = dispatcher->AddRegistry(3, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		CommandMessage * cmd = new CommandMessage(1);
		packet = new ACPPacket(3, 1, 3, 0, cmd);
		delete cmd;

		cout << "Task 3 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync1);
		if ((true != xSemaphoreTake(&sync3, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 3 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 3 Listening" << endl;
		for (i = 0; i < 100; ++i)
		{
			if (dispatcher->Listen(3))
			{
				break;
			}
			usleep(100000);
		}

		cout << "Task 3 Listen Done" << endl;
		if (i == 100)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 3 Waiting for Response" << endl;
		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}


		cout << "Task 3 Response Received" << endl;
		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 11 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		delete packet;

		xSemaphoreGive(&done3);
	}

	pthread_exit(0);
}

TEST(TestDispatcher, LotsOfCommands)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");

	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync1));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync2));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync3));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&done2));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&done3));

	ASSERT_FALSE(true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&sync3, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&done2, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&done3, MAX_BLOCK_TIME, 0));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_t testThread3;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);
	pthread_create(&testThread3, NULL, &taskTest3, NULL);

	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);
	pthread_join(testThread3, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);


	sem_destroy(&sync1);
	sem_destroy(&sync2);
	sem_destroy(&sync3);
	sem_destroy(&done2);
	sem_destroy(&done3);

}
