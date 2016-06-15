///*! \file SingleDataMessage.cpp
// *  \brief Implementation File for the SingleDataMessage Class.
// *
// *  This class defines a SingleDataMessage, which is any message that contains a single
// *  VariableTypeData field (DataMessage and ErrorMessage are derivatives).
// *
// */
//
//#include "core/SingleDataMessage.h"
//
//using namespace std;
//
//namespace Phoenix
//{
//	namespace Core
//	{
//		SingleDataMessage::SingleDataMessage()
//						  :SendMessage(), data()
//	    {
//			// Intentionally left empty
//	    }
//
//		SingleDataMessage::SingleDataMessage(MessageCodeType opcodeIn)
//							:SendMessage(opcodeIn), data()
//		{
//			// Intentionally left empty
//		}
//
//		SingleDataMessage::SingleDataMessage(MessageCodeType opcodeIn, const VariableTypeData & dataIn)
//							:SendMessage(opcodeIn), data(dataIn)
//		{
//			// Intentionally left empty
//		}
//
//		SingleDataMessage::SingleDataMessage(const SingleDataMessage & source)
//							:SendMessage(source), data(source.data)
//		{
//			// Intentionally left empty
//		}
//
//		SingleDataMessage::~SingleDataMessage(void )
//		{
//			// Intentionally left empty
//		}
//
//		VariableTypeData & SingleDataMessage::GetData(void)
//		{
//			return data;
//		}
//
//		bool SingleDataMessage::SetData(const VariableTypeData & newData)
//		{
//			// Attempt to set the data, returning false if any exceptions arise.
//			try
//			{
//				data = newData;
//			}
//			catch (exception e)
//			{
//				return false;
//			}
//
//			return true;
//		}
//
//		bool SingleDataMessage::SetData(uint8 * buffer, size_t size)
//		{
//			// Get the length from the buffer (2 bytes)
//			uint16 length = *buffer++;
//			length = length << 8;
//			length |= *buffer++;
//
//			// Update the size
//			size -= sizeof(uint16);
//
//			// Create a VariableTypeData from the buffer
//			VariableTypeData vtd(buffer, size);
//
//			// If length is not equal to the size of the VariableTypeData, return false
//			if (length != vtd.GetFlattenSize())
//			{
//				return false;
//			}
//
//			// Set the data to the VariableTypeData and return the result
//			return SetData(vtd);
//		}
//
//		SingleDataMessage & SingleDataMessage::operator=(const SingleDataMessage & source)
//		{
//			// Make sure this isn't the same as source
//			if (this == &source)
//			{
//				return *this;
//			}
//
//			SendMessage::operator=(source);
//
//			data = source.data;
//
//			return *this;
//		}
//
//
//		size_t SingleDataMessage::GetFlattenSize(void ) const
//		{
//			// Get flatten size of the parent class
//			size_t size = SendMessage::GetFlattenSize();
//
//			// Add size of the length field
//			size += sizeof(uint16);
//
//			// Add size of the data
//			size += data.GetFlattenSize();
//
//			return size;
//		}
//
//		bool SingleDataMessage::operator==(Message * check) const
//		{
//			// Attempt to cast check into a SingleDataMessage
//			SingleDataMessage * msg = dynamic_cast<SingleDataMessage *>(check);
//
//			// If unsuccessful, return false
//			if (NULL == msg)
//			{
//				return false;
//			}
//			else
//			{
//				// Check the parent class == operator, and if the data are the same.
//				return ((SendMessage::operator==(check)) &&
//						(data == msg->data));
//			}
//		}
//
//		size_t SingleDataMessage::Flatten(uint8 * buffer, size_t size) const
//		{
//			size_t numCopied = 0;
//
//			// Check the buffer and buffer size.
//			if (NULL == buffer || size < GetFlattenSize())
//			{
//				return 0;
//			}
//
//			// Call the parent class flatten, and update numCopied
//			numCopied = SendMessage::Flatten(buffer, size);
//
//			// Update the buffer and the size
//			buffer += numCopied;
//			size -= numCopied;
//
//			// Length of the message is equal to size of the data
//			uint16 length = data.GetFlattenSize();
//
//			// Put the length field into the buffer
//			*buffer++ = (length >> 8) & 0xFF;
//			*buffer++ = length & 0xFF;
//
//			// Update numCopied and the size
//			numCopied += sizeof(uint16);
//			size -= sizeof(uint16);
//
//			// Flatten the data and update numCopied
//			numCopied += data.Flatten(buffer, size);
//
//			return numCopied;
//		}
//	}
//}
