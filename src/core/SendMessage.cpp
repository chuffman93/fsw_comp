/*! \file SendMessage.cpp
 *  \brief Implementation File for the SendMessage Class.
 *
 *  This class defines a SendMessage, which is any message that is originally sent from
 *  one server to another, as opposed to being returned, as in a ReturnMessage.
 *
 */

#include "core/SendMessage.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
		SendMessage::SendMessage()
					:opcode(0)
		{
			// Intentionally left empty
		}

        SendMessage::SendMessage(MessageCodeType opcodeIn)
					:opcode(opcodeIn)
        {
        	// Intentionally left empty
        }

        SendMessage::SendMessage(const SendMessage & source)
					:opcode(source.opcode)
        {
        	// Intentionally left empty
        }
        
        SendMessage::~SendMessage(void )
		{
			//Intentionally left empty
		}

        MessageCodeType SendMessage::GetOpcode(void ) const
		{
			return opcode;
		}

        bool SendMessage::operator ==(Message * check) const
		{
			// Attempt to cast check into a SendMessage
			SendMessage * msg = dynamic_cast<SendMessage *>(check);

			// If unsuccessful, return false
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				// Check that the opcodes are the same, and the types are the same.
				return ((opcode == msg->opcode) &&
						(this->GetType() == check->GetType()));
			}
		}

        SendMessage & SendMessage::operator=(const SendMessage & source)
        {
        	// Make sure this isn't the same as source
        	if (this == &source)
        	{
        		return *this;
        	}

        	opcode = source.opcode;

        	return *this;
        }
        
        size_t SendMessage::GetFlattenSize(void ) const
        {
        	// Size of byte containing success, response, and type
        	size_t size = sizeof(uint8);

        	// Size of byte containing opcode
        	size += sizeof(MessageCodeType);

        	return size;
        }

        size_t SendMessage::Flatten(uint8 * buffer, size_t size) const
        {
        	size_t numCopied = 0;

			// Check the buffer and buffer size.
			if (NULL == buffer || size < GetFlattenSize())
			{
				return 0;
			}

			// If response is set, set the first byte to 1000000, else 00000000
			uint8 byte1 = (IsResponse() ? (1 << 7) : 0);
			// If success is set, set the first byte to X1000000, else X0000000
			byte1 |= (GetSuccess() ? (1 << 6) : 0);
			// Set first byte to XX(type - 6 bits)
			byte1 |= GetType() & 0x3f;

			// Put the first byte into the buffer, and update numCopied and the size
			*(buffer++) = byte1;
			++numCopied;
			--size;

			// Put the opcode into the buffer, and update numCopied and the size
			*(buffer++) = opcode;
			++numCopied;
			--size;

			return numCopied;
        }
	}
}
