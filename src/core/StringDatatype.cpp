/*! \file StringDatatype.cpp
 *  \brief Implementation File for the StringDatatype Class.
 *
 *  This class defines a wrapper class for a string.
 */

#include <cstring>

#include "core/StringDatatype.h"
#include "stdio.h"
using namespace std;

namespace Phoenix
{
	namespace Core
	{
		StringDatatype::StringDatatype(void )
		              : data(new string(""))
		{
			// Left Intentionally Empty
		}

		StringDatatype::StringDatatype(std::string dataIn)
		              : data(new string(dataIn))
		{
			// Left Intentionally Empty
		}

		StringDatatype::StringDatatype(uint8 * buffer, std::size_t size)
	    {
			size_t dataSize;

			// Check the buffer and the size.
			if (NULL == buffer || size < sizeof(uint32))
			{
				data = NULL;
				return;
			}

			// Get the string size from the buffer.
			dataSize = (buffer[0] << 24) | (buffer[1] << 16) |
					   (buffer[2] << 8)  | (buffer[3]);
			size -= sizeof(uint32);
			buffer += sizeof(uint32);

			// Check to make sure that there is enough data in the buffer
			// to match the size of the string.
			if (size < dataSize)
			{
				// The buffer is too small, so don't construct a string
				// from it.
				data = NULL;
			}
			else
			{
				// Construct a string from the buffer.
				data = new string((char *)buffer, dataSize);

				//printf("\r\n String Constructed of size %d\r\n",data->size());
			}
	    }

		StringDatatype::StringDatatype(const StringDatatype & source)
		{
			data = new string(*source.data);
		}

		StringDatatype::~StringDatatype(void)
		{
			delete data;
		}

		StringDatatype & StringDatatype::operator =(const StringDatatype & source)
		{
			delete data;
			data = new string(*source.data);
			return *this;
		}

		void * StringDatatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool StringDatatype::operator ==(Datatype * check) const
		{
			StringDatatype * msg = dynamic_cast<StringDatatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

	    std::size_t StringDatatype::GetFlattenSize(void) const
	    {
	    	return data->size( )+sizeof(uint32);
	    }

	    Datatype *StringDatatype::Duplicate(void) const
		{
	    	return new StringDatatype(*data);
		}

	    std::size_t StringDatatype::Flatten(uint8 *buffer, std::size_t size) const
	    {
	    	// Check the buffer and the size.
	    	if (NULL == buffer || size < data->size( ) + sizeof(uint32))
	    	{
	    		// The buffer is too small, so don't copy anything.
				return 0;
	    	}
	    	else
	    	{
	    		// Copy the size.
	    		size_t dataSize = data->size( );
				buffer[0] = (dataSize >> 24) & 0xff;
				buffer[1] = (dataSize >> 16) & 0xff;
				buffer[2] = (dataSize >> 8) & 0xff;
				buffer[3] = (dataSize) & 0xff;
				buffer += sizeof(uint32);

				// Copy the data.
	    		memcpy(buffer, data->c_str( ), dataSize);

	    		printf("\r\nPrinting the String packet \r\n");
	    		for (int pq =0 ;pq<dataSize;pq++)
	    			printf("\r\n %d",buffer[pq]);
				return data->size( )+sizeof(uint32);
	    	}
	    }
	}
}
