/*! \file ReturnMessage.cpp
 *  \brief Implementation File for the ReturnMessage Class.
 *
 *  This class defines a ReturnMessage, which is any message that is sent as a
 *  response from one server to another, as opposed to being originally sent,
 *  as in a SendMessage.
 *
 */

#include "core/ReturnMessage.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
		ReturnMessage::ReturnMessage(void )
		              : messagePtr(NULL), success(false)
		{
			// Intentionally left empty
		}
        ReturnMessage::ReturnMessage(Message * messageIn, bool successIn)
					  : success(successIn)
		{
        	// Copy the pointer if it is not null
        	if (NULL != messageIn)
        	{
        		messagePtr = messageIn->Duplicate();
        	}
        	// Else set the pointer to null
        	else
        	{
        		messagePtr = NULL;
        	}
		}

        ReturnMessage::ReturnMessage(const ReturnMessage & source)
					  : success(source.success)
        {
        	// Copy the pointer if it is not null
        	if (NULL != source.messagePtr)
			{
				messagePtr = source.messagePtr->Duplicate();
			}
        	// Else set the pointer to null
			else
			{
				messagePtr = NULL;
			}
        }
        
        ReturnMessage::~ReturnMessage(void )
		{
        	// Memory management of the pointer
			delete messagePtr;
			messagePtr = NULL;
		}

        bool ReturnMessage::IsResponse() const
		{
			return true;
		}

		bool ReturnMessage::GetSuccess(void ) const
		{
			return success;
		}

		void ReturnMessage::SetSuccess(bool newSuccess)
		{
			success = newSuccess;
		}

		MessageTypeEnum ReturnMessage::GetType(void ) const
		{
			// Return type from the pointer
			return messagePtr->GetType( );
		}

		MessageCodeType ReturnMessage::GetOpcode(void ) const
		{
			// Return opcode from the pointer
			return messagePtr->GetOpcode( );
		}

		Message * ReturnMessage::GetMessagePtr(void ) const
		{
			return messagePtr;
		}

        bool ReturnMessage::operator ==(Message * check) const
		{
        	// Attempt to cast into a ReturnMessage
			ReturnMessage * msg = dynamic_cast<ReturnMessage *>(check);

			// Check if unsuccessful
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				// Check case for if both pointers are null so that null
				// is not dereferenced later on.
				if (messagePtr == NULL && (msg->messagePtr) == NULL)
				{
					// Check the messages are the same
					return (success == msg-> success);
				}

				// Compare the pointers, and successes
				return ((messagePtr->operator == (msg->messagePtr)) &&
						(success == msg->success));
			}
		}
        
        ReturnMessage & ReturnMessage::operator=(const ReturnMessage & source)
		{
        	// Make sure this and source aren't the same
			if (this == &source)
			{
				return *this;
			}

			// Memory management of pointer
			delete messagePtr;

			// If pointer isn't null, copy it
			if (NULL != source.messagePtr)
			{
				messagePtr = source.messagePtr->Duplicate();
			}
			// Else set pointer to null
			else
			{
				messagePtr = NULL;
			}

			success = source.success;

			return *this;
		}

        Message * ReturnMessage::Duplicate() const
		{
			return new ReturnMessage(*this);
		}

        size_t ReturnMessage::GetFlattenSize(void ) const
        {
        	// ReturnMessage with null pointer flattens to 0
        	if (messagePtr == NULL)
        	{
        		return 0;
        	}

        	// Get flatten size of the pointer
        	size_t size = messagePtr->GetFlattenSize();

        	return size;
        }
        


        size_t ReturnMessage::Flatten(uint8 * buffer, size_t size) const
        {
        	// ReturnMessage with null pointer flattens to 0
        	if (messagePtr == NULL)
        	{
        		return 0;
        	}

        	size_t numCopied = 0;

			// Check the buffer and buffer size.
			if (NULL == buffer || size < GetFlattenSize())
			{
				return 0;
			}

			// Flatten the message pointer
			numCopied = messagePtr->Flatten(buffer, size);

			// Pull the first byte from the buffer
			uint8 byte1 = *buffer;

			// Set the 1st leftmost bit (response)
			byte1 = byte1 | 0x80;

			// Set the 2nd leftmost bit to success variable
			byte1 = byte1 | (GetSuccess() << 6);

			// Put the edited first byte back in
			*buffer = byte1;

        	return numCopied;
        }
    }
}
