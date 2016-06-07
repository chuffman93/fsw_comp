/*! \file Message.cpp
 *  \brief Implementation File for the Message Class.
 *
 *  This class encapsulates the message field of a Phoenix message protocol
 *  packet, and defines a superclass for the specific derivatives of Message to
 *  inherit from.
 *
 */

#include "core/Message.h"
#include "core/CommandMessage.h"
#include "core/DataMessage.h"
#include "core/ErrorMessage.h"
#include "core/ConfigMessage.h"
#include "core/ReturnMessage.h"
#include <stdio.h>

using namespace std;

namespace Phoenix
{
    namespace Core
    {
        Message * Message::CreateMessage(uint8 * buffer, size_t size)
		{
        	// Variables to hold data as it is extracted from the buffer
        	Message * ret;

        	MessageTypeEnum type;

        	uint8 byte1;

        	bool response;
        	bool success;
			bool rv = true;

        	MessageCodeType opCode;

        	// Check that the buffer is sized correctly.
        	if (NULL == buffer || size < 2)
			{
				return NULL;
			}

        	//Get the first byte from the buffer, and update the size.
        	byte1 = (*(buffer++));
        	size--;

			//Get the response bit from the first byte (X*******) -> (0000000X).
        	response = (byte1 >> 7) & 0x01;

			//Get the success bit from the first byte (*Y******) -> (0000000Y).
        	success = (byte1 >> 6) & 0x01;

        	//Get the message type (6 bits) from the first byte (**ZZZZZZ) -> (00ZZZZZZ).
        	type = (MessageTypeEnum)(byte1 & 0x3F);

        	//Get the opcode from the buffer, and update the size.
        	opCode = (MessageCodeType)(*(buffer++));
        	size--;

			//Construct the message depending on the type, and pass the rest of the buffer
        	//for the message to set its information
        	switch (type)
        	{
				case MESSAGE_TYPE_COMMAND:
					ret = new CommandMessage(opCode);
					printf("\r\nCommand Message generated\r\n");
					rv = ((CommandMessage *)ret)->SetParameters(buffer,size);
					break;
				case MESSAGE_TYPE_DATA:
					ret = new DataMessage(opCode);
					rv = ((DataMessage *)ret)->SetParameters(buffer,size);
					break;
				case MESSAGE_TYPE_ERROR:
					ret = new ErrorMessage(opCode);
					rv = ((ErrorMessage *)ret)->SetParameters(buffer,size);
					break;
				case MESSAGE_TYPE_CONFIG:
					ret = new ConfigMessage(opCode);
					rv = ((ConfigMessage *)ret)->SetParameters(buffer,size);
					break;
				default:
					return NULL;
        	}
			if(rv == false)
			{
				printf("Invalid parameters");
				delete ret;
				return NULL;
			}

        	Message * tmp;

        	//If this message is a response, create a new ReturnMessage
        	if (response)
        	{
        		printf("Creating return message\n");
        		//Create new ReturnMessage with prior message and success as the parameters
        		tmp = new ReturnMessage(ret,success);
        		delete ret;
        		ret = tmp;
        	}

        	printf("Returning\n");
        	return ret;
		}
    }
}
