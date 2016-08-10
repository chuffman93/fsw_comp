/*! \file Uint32Datatype.cpp
 *  \brief Implementation File for the Uint32Datatype Class.
 *
 *  This class defines a wrapper class for a 32-bit unsigned integer value.
 */

#include "core/Uint32Datatype.h"

namespace AllStar
{
	namespace Core
	{
		Uint32Datatype::Uint32Datatype(void )
		              : data(new uint32(0))
		{
			// Left Intentionally Empty
		}

		Uint32Datatype::Uint32Datatype(uint32 dataIn)
		              : data(new uint32(dataIn))
		{
			// Left Intentionally Empty
		}

		Uint32Datatype::Uint32Datatype(uint8 * buffer, std::size_t size)
	    {
			// Check the buffer and the size.
			if (NULL == buffer || size < sizeof(uint32))
			{
				data = NULL;
				return;
			}

			// Create the unsigned integer from the buffer.
			data = new uint32((uint32)((buffer[0] << 24) |
									   (buffer[1] << 16) |
									   (buffer[2] << 8)  |
									   (buffer[3])));
	    }

		Uint32Datatype::Uint32Datatype(const Uint32Datatype & source)
		{
			data = new uint32(*source.data);
		}

		Uint32Datatype::~Uint32Datatype(void )
		{
			delete data;
		}

		Uint32Datatype & Uint32Datatype::operator =(const Uint32Datatype & source)
		{
			delete data;
			data = new uint32(*source.data);
			return *this;
		}

		void * Uint32Datatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool Uint32Datatype::operator ==(Datatype * check) const
		{
			Uint32Datatype * msg = dynamic_cast<Uint32Datatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

	    std::size_t Uint32Datatype::GetFlattenSize(void) const
	    {
	    	return sizeof(uint32);
	    }

	    Datatype * Uint32Datatype::Duplicate(void) const
		{
	    	return new Uint32Datatype(*data);
		}

	    std::size_t Uint32Datatype::Flatten(uint8 *buffer, std::size_t size) const
	    {
	    	// Check the buffer and the size.
	    	if (NULL == buffer || size < sizeof(uint32))
	    	{
	    		// The buffer is too small, so don't copy anything.
				return 0;
	    	}
	    	else
	    	{
	    		// Copy the data.
	    		buffer[0] = (uint8)((*data >> 24) & 0xff);
				buffer[1] = (uint8)((*data >> 16) & 0xff);
				buffer[2] = (uint8)((*data >> 8) & 0xff);
				buffer[3] = (uint8)(*data & 0xff);
				return sizeof(uint32);
	    	}
	    }
	}
}
