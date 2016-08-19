/* testdisp0005.cpp
 * Created on: Jun 12, 2014
 *    Author: fsw
 *  \brief Test 5 of the Dispatcher Class.
 *
 *  This test verifies that if a packet is sent to a server that is listening
 *  but has not yet added a registry to the Dispatcher, Listen sends an
 *  ERROR_OPCODE_SERVER_NOT_REGISTERED ErrorMessage.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace AllStar::Core;

class TestMessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler5;

//ReturnMessage * TestMessageHandler::Handle(const ACPPacket & packet)
//{
//	CommandMessage msg(0);
//	ReturnMessage * ret = new ReturnMessage(&msg, true);
//	return ret;
//}

static sem_t sync0;

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
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Initialized" << endl;

		CommandMessage * cmd = new CommandMessage(0);
		packet = new ACPPacket(1, 2, 0, 0, cmd);
		delete cmd;

		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE();
			pthread_exit(0);
		}


		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE();
			pthread_exit(0);
		}

		if (retMsg.GetSuccess() ||
			retMsg.GetOpcode() != ERROR_OPCODE_SERVER_NOT_REGISTERED ||
			retMsg.GetType() != MESSAGE_TYPE_ERROR)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE();
			pthread_exit(0);
		}

		delete packet;
	}

	xSemaphoreTake(&sync0, MAX_BLOCK_TIME, 0);

	pthread_exit(0);
}

static void * taskTest2(void * parameters)
{
	{
		Dispatcher * dispatcher;
		MessageHandlerRegistry reg;
		size_t i;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

		cout << "Initialized" << endl;

		for (i = 0; i < 10; ++i)
		{
			cout << i << " : i equals" << endl;
			if (dispatcher->Listen(2))
			{
				ADD_FAILURE();
				pthread_exit(0);
			}
			usleep(10000);
		}

		if (i != 10)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}

	}

	xSemaphoreGive(&sync0);

	pthread_exit(0);
}

TEST(TestDispatcher, NoAddedReg)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");

	ASSERT_TRUE(vSemaphoreCreateBinary(&sync0) == true);
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
