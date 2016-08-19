/* testdisp0006.cpp
 * Created on: Jun 13, 2014
 *    Author: fsw
 *  \brief Test 6 of the Dispatcher Class.
 *
 *  This test verifies that if a packet is sent to a server that is listening
 *  and has added a handler registry but the message does not match any
 *  registered handlers, then Listen sends an
 *  ERROR_OPCODE_UNRECOGNIZED_TYPE ErrorMessage.
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

class TestMessageHandler : public MessageHandler
{
public:
	ReturnMessage * Handle(const ACPPacket & packet);
} testHandler6;

//ReturnMessage * TestMessageHandler::Handle(const ACPPacket & packet)
//{
//	CommandMessage msg(0);
//	ReturnMessage * ret = new ReturnMessage(&msg, true);
//	return ret;
//}

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

		CommandMessage * cmd = new CommandMessage(1);
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
			retMsg.GetOpcode() != ERROR_OPCODE_UNRECOGNIZED_TYPE ||
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
		reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), &testHandler6);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 0), true);
		arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, 1), true);
		ret = dispatcher->AddRegistry(2, &reg, &arby);
		if (!ret)
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
			usleep(1000000);
		}

		if (i == 10)
		{
			ADD_FAILURE();
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}


TEST(TestDispatcher, OP_UNRECOGNIZED)
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
