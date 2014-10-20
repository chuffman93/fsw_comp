/*! \file Int32Datatype.cpp
 *  \brief Implementation File for the Int32Datatype Class.
 *
 *  This class defines a wrapper class for a 32-bit signed integer value.
 */

#include "core/Int32Datatype.h"

namespace Phoenix
{
	namespace Core
	{
		Int32Datatype::Int32Datatype(void )
		              : data(new int32(0))
		{
			// Left Intentionally Empty
		}

		Int32Datatype::Int32Datatype(int32 dataIn)
		              : data(new int32(dataIn))
		{
			// Left Intentionally Empty
		}

		Int32Datatype::Int32Datatype(uint8 * buffer, std::size_t size)
	    {
			// Check the buffer and the size.
			if (NULL == buffer || size < sizeof(int32))
			{
				data = NULL;
				return;
			}

			data = new int32((int32)((buffer[0] << 24) |
					                 (buffer[1] << 16) |
					                 (buffer[2] << 8)  |
					                 (buffer[3])));
	    }

		Int32Datatype::Int32Datatype(const Int32Datatype & source)
		{
			data = new int32(*source.data);
		}

		Int32Datatype::~Int32Datatype()
		{
			delete data;
		}

		Int32Datatype & Int32Datatype::operator =(const Int32Datatype & source)
		{
			delete data;
			data = new int32(*source.data);
			return *this;
		}

		void * Int32Datatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool Int32Datatype::operator ==(Datatype * check) const
		{
			Int32Datatype * msg = dynamic_cast<Int32Datatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

	    std::size_t Int32Datatype::GetFlattenSize(void) const
	    {
	    	return sizeof(int32);
	    }

	    Datatype *Int32Datatype::Duplicate(void) const
		{
	    	return new Int32Datatype(*data);
		}

	    std::size_t Int32Datatype::Flatten(uint8 *buffer, std::size_t size) const
	    {
	    	// Check the buffer and the size.
	    	if (NULL == buffer || size < sizeof(int32))
	    	{
	    		// The buffer is not big enough, so don't copy anything.
				return 0;
	    	}
	    	else
	    	{
	    		// Copy the data.
	    		buffer[0] = (uint8)((*data >> 24) & 0xff);
				buffer[1] = (uint8)((*data >> 16) & 0xff);
				buffer[2] = (uint8)((*data >> 8) & 0xff);
				buffer[3] = (uint8)(*data & 0xff);
				return sizeof(int32);
	    	}
	    }
	}
}
