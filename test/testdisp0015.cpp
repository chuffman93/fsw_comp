/* testdisp0015.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 15 of the Dispatcher Class.
 *
 *  This test verifies that if a packet is sent to a server that is listening
 *  and has added a handler registry but no permission has been registered
 *  for that message, then Listen sends an ERROR_OPCODE_PACKET_NOT_ALLOWED
 *  ErrorMessage.  It also checks that if a permission is registered for the
 *  message but the message is not allowed, the same response is sent.
 *  Finally, the test verifies that if a message is given permissions
 *  for a given server, then the message is handled by Dispatcher.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/CommandMessage.h"
#include "core/ErrorMessage.h"

using namespace std;
using namespace AllStar::Core;

class Test0015MessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler15;

ReturnMessage * Test0015MessageHandler::Handle(const ACPPacket & packet)
{
	CommandMessage msg(0);
	ReturnMessage * ret = new ReturnMessage(&msg, true);
	return ret;
}

static sem_t sync1, sync2, sync3;

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
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Initialized" << endl;

		CommandMessage * cmd = new CommandMessage(0);
		packet = new ACPPacket(1, 2, 0, 0, cmd);
		delete cmd;
		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		xSemaphoreGive(&sync1);

		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (retMsg.GetSuccess() ||
			retMsg.GetOpcode() != ERROR_OPCODE_PACKET_NOT_ALLOWED ||
			retMsg.GetType() != MESSAGE_TYPE_ERROR)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << (uint8)retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		xSemaphoreGive(&sync2);

		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (retMsg.GetSuccess() ||
			retMsg.GetOpcode() != ERROR_OPCODE_PACKET_NOT_ALLOWED ||
			retMsg.GetType() != MESSAGE_TYPE_ERROR)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << (uint8)retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (!dispatcher->Dispatch(*packet))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		xSemaphoreGive(&sync3);

		if (DISPATCHER_STATUS_OK != (status = dispatcher->WaitForDispatchResponse(*packet, retMsg)))
		{
			cout << "Status: " << status << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		if (!retMsg.GetSuccess() ||
			retMsg.GetOpcode() !=  0 ||
			retMsg.GetType() != MESSAGE_TYPE_COMMAND)
		{
			cout << "Success: " << retMsg.GetSuccess()
				 << "  Opcode: " << (uint32)retMsg.GetOpcode()
				 << "  Type: " << retMsg.GetType() << endl;
			ADD_FAILURE(); pthread_exit(0);
		}

		delete packet;
	}
	
	if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
	{
		ADD_FAILURE(); pthread_exit(0);
	}

	pthread_exit(0);
}

static void * taskTest2(void * parameters)
{
	{
		Dispatcher * dispatcher;
		MessageHandlerRegistry reg;
		Arbitrator arby(2);
		Permission p1;
		bool ret;
		size_t i;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Initialized" << endl;
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler15);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		if ((true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		if (dispatcher->Listen(2))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), p1);

		if ((true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		if (dispatcher->Listen(2))
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		p1.SetServerPermission(1, true);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), p1);

		if ((true != xSemaphoreTake(&sync3, MAX_BLOCK_TIME, 0)))
		{
			ADD_FAILURE(); pthread_exit(0);
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

	xSemaphoreGive(&sync1);

	pthread_exit(0);
}



TEST(TestDispatcher, ERROR_OPCODE_NOT_ALLOWED)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");

	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync1));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync2));
	ASSERT_TRUE(true == vSemaphoreCreateBinary(&sync3));

	ASSERT_FALSE(true != xSemaphoreTake(&sync1, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&sync2, MAX_BLOCK_TIME, 0));
	ASSERT_FALSE(true != xSemaphoreTake(&sync3, MAX_BLOCK_TIME, 0));

	pthread_t testThread1;
	pthread_t testThread2;
	pthread_create(&testThread1, NULL, &taskTest1, NULL);
	pthread_create(&testThread2, NULL, &taskTest2, NULL);


	pthread_join(testThread1, NULL);
	pthread_join(testThread2, NULL);

	Factory::Destroy(DISPATCHER_SINGLETON);


	sem_destroy(&sync1);
	sem_destroy(&sync2);
	sem_destroy(&sync3);

}
