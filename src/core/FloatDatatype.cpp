/*! \file FloatDatatype.cpp
 *  \brief Implementation File for the FloatDatatype Class.
 *
 *  This class defines a wrapper class for a single-precision floating point
 *  value.
 */

#include "core/FloatDatatype.h"

namespace Phoenix
{
	namespace Core
	{
		FloatDatatype::FloatDatatype(void )
		              : data(new float(0))
		{
			// Left Intentionally Empty
		}

		FloatDatatype::FloatDatatype(float dataIn)
		              : data(new float(dataIn))
		{
			// Left Intentionally Empty
		}

		FloatDatatype::FloatDatatype(uint8 * buffer, std::size_t size)
	    {
			uint32 tmp;

			// Check the buffer and the size.
			if (NULL == buffer || size < sizeof(float))
			{
				data = NULL;
				return;
			}

			//data = new float(*reinterpret_cast<float *>(buffer));
			tmp = ((uint32)((buffer[0] << 24) |
						    (buffer[1] << 16) |
						    (buffer[2] << 8)  |
						    (buffer[3])));
			data = new float(*reinterpret_cast<float *>(&tmp));
	    }

		FloatDatatype::FloatDatatype(const FloatDatatype & source)
		{
			data = new float(*source.data);
		}

		FloatDatatype::~FloatDatatype(void )
		{
			delete data;
		}

		FloatDatatype & FloatDatatype::operator =(const FloatDatatype & source)
		{
			delete data;
			data = new float(*source.data);
			return *this;
		}

		void * FloatDatatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool FloatDatatype::operator ==(Datatype * check) const
		{
			FloatDatatype * msg = dynamic_cast<FloatDatatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

	    std::size_t FloatDatatype::GetFlattenSize(void) const
	    {
	    	return sizeof(float);
	    }

	    Datatype *FloatDatatype::Duplicate(void) const
		{
	    	return new FloatDatatype(*data);
		}

	    std::size_t FloatDatatype::Flatten(uint8 *buffer, std::size_t size) const
	    {
	    	// Check the buffer and the size.
	    	if (NULL == buffer || size < sizeof(float))
	    	{
	    		// The buffer is too small, so don't copy anything.
	    		return 0;
	    	}
	    	else
	    	{
	    		// Copy the data.
	    		//memcpy(buffer,reinterpret_cast<uint8 *>(data),sizeof(float));
	    		uint32 * tmp = reinterpret_cast<uint32 *>(data);
	    		buffer[0] = (uint8)((*tmp >> 24) & 0xff);
				buffer[1] = (uint8)((*tmp >> 16) & 0xff);
				buffer[2] = (uint8)((*tmp >> 8) & 0xff);
				buffer[3] = (uint8)(*tmp & 0xff);

				return sizeof(float);
	    	}
	    }
	}
}
