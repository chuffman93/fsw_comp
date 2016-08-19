/* testdisp0012.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 12 of the Dispatcher Class.
 *
 *  This test verifies that when a Dispatched packet is not received by the
 *  destination server, Wait correctly returns DISPATCHER_STATUS_MSG_NOT_RCVD.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"

using namespace std;
using namespace AllStar::Core;

class Test0012MessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler12;

ReturnMessage * Test0012MessageHandler::Handle(const ACPPacket & packet)
{
	CommandMessage msg(packet.GetMessagePtr()->GetOpcode()+10);
	ReturnMessage * ret = new ReturnMessage(&msg, true);
	return ret;
}

static sem_t sync0;

static void * taskTest1(void * parameters)
{
	{
		Dispatcher * dispatcher;
		ACPPacket * packet1;
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

		if (DISPATCHER_STATUS_MSG_NOT_RCVD != (status = dispatcher->WaitForDispatchResponse(*packet1, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Task 1 Done" << endl;
		delete packet1;
	}

	xSemaphoreGive(&sync0);

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
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler12);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), true);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Registry Added" << endl;

		if ((true != xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0)))
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

		if (i != 10)
		{
			ADD_FAILURE(); pthread_exit(0);
		}
	}

	pthread_exit(0);

	
}



TEST(TestDispatcher, DISPATCHER_STATUS_MSG_NOT_RCVD)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");

	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync0));

	ASSERT_TRUE(true == xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0));


	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);


	sem_destroy(&sync0);
}
