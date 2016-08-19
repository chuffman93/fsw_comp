/* testdisp0016.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 16 of the Dispatcher Class.
 *
 *  This test verifies that if a message handler dispatches messages to other
 *  servers, then Dispatcher correctly sends the messages and responses.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"
using namespace std;
using namespace AllStar::Core;

class Test0016MessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler16;

class ResponseMessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} respHandler;

ReturnMessage * Test0016MessageHandler::Handle(const ACPPacket & packet)
{
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	DispatcherStatusEnum status;
	CommandMessage * cmd = new CommandMessage(1);
	ACPPacket * outPacket = new ACPPacket(2, 1, 0, 0, cmd);
	ReturnMessage retMsg;
	delete cmd;

	cout << "Handler Dispatching" << endl;
	if (!dispatcher->Dispatch(*outPacket))
	{
		ADD_FAILURE(); pthread_exit(0);
	}

	cout << "Handler Waiting for Response" << endl;
	if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*outPacket, retMsg)))
	{
		cout << "Status: " << status << endl;
		ADD_FAILURE(); pthread_exit(0);
	}

	cout << "Handler Response Received" << endl;
	if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 11 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
	{
		cout << "Success: " << retMsg.GetSuccess()
			 << "  Opcode: " << retMsg.GetOpcode()
			 << "  Type: " << retMsg.GetType() << endl;
		ADD_FAILURE(); pthread_exit(0);
	}

	delete outPacket;

	CommandMessage msg(packet.GetMessagePtr()->GetOpcode()+10);
	ReturnMessage * ret = new ReturnMessage(&msg, true);
	return ret;
}

ReturnMessage * ResponseMessageHandler::Handle(const ACPPacket & packet)
{
	cout << "Response Message!" << endl;
	CommandMessage msg(packet.GetMessagePtr()->GetOpcode()+10);
	ReturnMessage * ret = new ReturnMessage(&msg, true);
	return ret;
}

static sem_t sync1, sync2, done2;

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
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), &respHandler);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), true);
		ret = dispatcher->AddRegistry(1, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		CommandMessage * cmd = new CommandMessage(2);
		packet = new ACPPacket(1, 2, 0, 0, cmd);
		delete cmd;

		cout << "Task 1 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync2);
		if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Dispatching" << endl;
		if (!dispatcher->Dispatch(*packet))
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

		cout << "Task 1 Waiting for Response" << endl;
		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
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

		if ((true != xSemaphoreTake(&done2, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
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
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 2), &testHandler16);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 2), true);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 2 Waiting for Synchronization" << endl;
		xSemaphoreGive(&sync1);
		if ((true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0)))
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
	}

	xSemaphoreGive(&done2);

	pthread_exit(0);
}



TEST(TestDispatcher, HandlerDispatch)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync1));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync2));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&done2));


	ASSERT_FALSE(true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&done2, MAX_BLOCK_TIME, 0));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_t testThread3;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);
	pthread_join(testThread3, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);


	sem_destroy(&sync1);
	sem_destroy(&sync2);
	sem_destroy(&done2);


}
