/*! \file DoubleDatatype.cpp
 *  \brief Implementation File for the DoubleDatatype Class.
 *
 *  This class defines a wrapper class for a double-precision floating point
 *  value.
 */

#include "core/DoubleDatatype.h"

namespace Phoenix
{
	namespace Core
	{
		DoubleDatatype::DoubleDatatype(void )
		              : data(new double(0))
		{
			// Left Intentionally Empty
		}

		DoubleDatatype::DoubleDatatype(double dataIn)
		              : data(new double(dataIn))
		{
			// Left Intentionally Empty
		}

		DoubleDatatype::DoubleDatatype(uint8 * buffer, std::size_t size)
	    {
			uint64 tmp;
			// Check the buffer and the size.
			if (NULL == buffer || size < sizeof(double))
			{
				data = NULL;
				return;
			}

			//data = new double(*reinterpret_cast<double *>(buffer));
			tmp = ((uint64)((((uint64)buffer[0]) << 56) |
							(((uint64)buffer[1]) << 48) |
							(((uint64)buffer[2]) << 40) |
							(((uint64)buffer[3]) << 32) |
							(buffer[4] << 24) |
							(buffer[5] << 16) |
							(buffer[6] << 8)  |
							(buffer[7])));
			data = new double(*reinterpret_cast<double *>(&tmp));
	    }

		DoubleDatatype::DoubleDatatype(const DoubleDatatype & source)
		{
			data = new double(*source.data);
		}

		DoubleDatatype::~DoubleDatatype(void )
		{
			delete data;
		}

		DoubleDatatype & DoubleDatatype::operator =(const DoubleDatatype & source)
		{
			delete data;
			data = new double(*source.data);
			return *this;
		}

		void * DoubleDatatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool DoubleDatatype::operator ==(Datatype * check) const
		{
			DoubleDatatype * msg = dynamic_cast<DoubleDatatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

	    std::size_t DoubleDatatype::GetFlattenSize(void) const
	    {
	    	return sizeof(double);
	    }

	    Datatype *DoubleDatatype::Duplicate(void) const
		{
	    	return new DoubleDatatype(*data);
		}

	    std::size_t DoubleDatatype::Flatten(uint8 *buffer, std::size_t size) const
	    {
	    	// Check the buffer and the size.
	    	if (NULL == buffer || size < sizeof(double))
	    	{
	    		// The buffer is not big enough, so don't copy anything.
	    		return 0;
	    	}
	    	else
	    	{
	    		// Copy the data.
	    		//memcpy(buffer,reinterpret_cast<uint8 *>(data),sizeof(double));
	    		uint64 * tmp = reinterpret_cast<uint64 *>(data);
				buffer[0] = (uint8)((*tmp >> 56) & 0xff);
				buffer[1] = (uint8)((*tmp >> 48) & 0xff);
				buffer[2] = (uint8)((*tmp >> 40) & 0xff);
				buffer[3] = (uint8)((*tmp >> 32) & 0xff);
				buffer[4] = (uint8)((*tmp >> 24) & 0xff);
				buffer[5] = (uint8)((*tmp >> 16) & 0xff);
				buffer[6] = (uint8)((*tmp >> 8)  & 0xff);
				buffer[7] = (uint8)(*tmp & 0xff);

				return sizeof(double);
	    	}
	    }
	}
}
