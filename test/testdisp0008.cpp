/* testdisp0008.cpp
 * Created on: Jun 24, 2014
 *    Author: fsw
 *  \brief Test 8 of the Dispatcher Class.
 *
 *  This test verifies that if two messages are Dispatched, then the messages
 *  are processed in the correct order.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

class Test0008MessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler8;

ReturnMessage * Test0008MessageHandler::Handle(const ACPPacket & packet)
{
	CommandMessage msg(packet.GetMessagePtr()->GetOpcode());
	ReturnMessage * ret = new ReturnMessage(&msg, true);
	return ret;
}

static sem_t sync0;

static void * taskTest1(void * parameters)
{
	{
		Dispatcher * dispatcher;
		ACPPacket * packet1, * packet2;
		ReturnMessage retMsg;
		DispatcherStatusEnum status;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Initialized" << endl;

		CommandMessage * cmd = new CommandMessage(0);
		packet1 = new ACPPacket(1, 2, 0, 0, cmd);
		delete cmd;

		if (!dispatcher->Dispatch(*packet1))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cmd = new CommandMessage(1);
		packet2 = new ACPPacket(1, 2, 1, 0, cmd);
		delete cmd;

		if (!dispatcher->Dispatch(*packet2))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet1, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 0 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}
		delete packet1;

		xSemaphoreGive(&sync0);

		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet2, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 1 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess() << endl;
			cout << "  Opcode: " << retMsg.GetOpcode() << endl;
			cout << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}
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
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler8);
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), &testHandler8);
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
			if (dispatcher->Listen(2))
			{
				break;
			}
			usleep(100000);
		}

		if (i == 10)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		if ((true != xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		for (i = 0; i < 10; ++i)
		{
			if (dispatcher->Listen(2))
			{
				break;
			}
			usleep(100000);
		}

		if (i == 10)
		{
			ADD_FAILURE(); pthread_exit(0);
		}
	}

	pthread_exit(0);
}


TEST(TestDispatcher, ProcessingOrder)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");

	ASSERT_TRUE(vSemaphoreCreateBinary(&sync0) == true );
	ASSERT_TRUE(true == xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0));


	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);
	sem_destroy(&sync0);
}
