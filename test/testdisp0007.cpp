/* testdisp0007.cpp
 * Created on: Jun 20, 2014
 *    Author: fsw
 *  \brief Test 7 of the Dispatcher Class.
 *
 *  This test verifies that if a server calls Dispatch too many times
 *  before a Listen happens, Dispatch will return false until Listen
 *  and Wait are called.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
using namespace std;



#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace AllStar::Core;

class TestMessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler7;

//ReturnMessage * TestMessageHandler::Handle(const ACPPacket & packet)
//{
//	CommandMessage msg(0);
//	ReturnMessage *ret = new ReturnMessage(&msg, true);
//	return ret;
//}

static sem_t sync1, sync2;

static void * taskTest1(void * parameters)
{
	{
		Dispatcher * dispatcher;
		ACPPacket * packet;
		ReturnMessage retMsg;
		size_t i;
		DispatcherStatusEnum status;
		CommandMessage * cmd = new CommandMessage(0);

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Initialized" << endl;

		for (i = 0; i < DISPATCHER_QUEUE_LENGTH; ++i)
		{
			cout << "placing packet with numberIn: " << i << endl;
			packet = new ACPPacket(1, 2, i, 0, cmd);
			if (!dispatcher->Dispatch(*packet))
			{
				ADD_FAILURE();
				pthread_exit(0);
			}
			delete packet;
		}

		packet = new ACPPacket(1, 2, DISPATCHER_QUEUE_LENGTH, 0, cmd);
		if (dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}
		delete packet;

		xSemaphoreGive(&sync2);
		//usleep(1000);
		if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		usleep(100000);

		packet = new ACPPacket(1, 2, DISPATCHER_QUEUE_LENGTH, 0, cmd);
		if (dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}
		delete packet;

		packet = new ACPPacket(1, 2, 0, 0, cmd);

		for (i = 0; i < DISPATCHER_QUEUE_LENGTH; ++i)
		{
			packet->SetNumber(i+9);
			cout << "Listening for packet number: " << i << endl;
			if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
			{
				cout << "Status: " << status << endl;
				ADD_FAILURE();
				pthread_exit(0);
			}

			if (!retMsg.GetSuccess() || retMsg.GetOpcode() != 0 || retMsg.GetType() != MESSAGE_TYPE_COMMAND)
			{
				cout << "Success: " << retMsg.GetSuccess()
					 << "  Opcode: " << retMsg.GetOpcode()
					 << "  Type: " << retMsg.GetType() << endl;
				ADD_FAILURE();
				pthread_exit(0);
			}
		}

		while ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
		{

		}

		delete packet;
		delete cmd;
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
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler7);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), true);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		xSemaphoreGive(&sync1);
		if ((true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		for (size_t j = 0; j < DISPATCHER_QUEUE_LENGTH; ++j)
		{
			for (i = 0; i < 10; ++i)
			{
				if (dispatcher->Listen(2))
				{
					break;
				}
			}
			usleep(100000);

			if (i == 10)
			{
				ADD_FAILURE();
				pthread_exit(0);
			}
		}
	}

	xSemaphoreGive(&sync1);
	cout << "exited!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1" << endl;
	pthread_exit(0);
}


TEST(TestDispatcher, DispatchOverload)
{
	
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	mq_unlink("/errQueueHandle");

	ASSERT_TRUE(vSemaphoreCreateBinary(&sync1) != 0); ASSERT_TRUE(vSemaphoreCreateBinary(&sync2) != 0);

	ASSERT_TRUE(true == xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0));
	ASSERT_TRUE(true == xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0));

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);

	pthread_join(testThread2, NULL);
	pthread_join(testThread1, NULL);

	Factory::Destroy(DISPATCHER_SINGLETON);


	sem_destroy(&sync1);
	sem_destroy(&sync2);

}
