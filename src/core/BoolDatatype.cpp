/*! \file BoolDatatype.cpp
 *  \brief Implementation File for the BoolDatatype Class.
 *
 *  This class defines a wrapper class for a Boolean value.
 */

#include "core/BoolDatatype.h"

namespace AllStar
{
	namespace Core
	{
		BoolDatatype::BoolDatatype(void )
		              : data(new bool(false))
		{
			// Left Intentionally Empty
		}

		BoolDatatype::BoolDatatype(bool dataIn)
		              : data(new bool(dataIn))
		{
			// Left Intentionally Empty
		}

		BoolDatatype::BoolDatatype(uint8 * buffer, std::size_t size)
	    {
			// Check the buffer and size.
			if (NULL == buffer || size < sizeof(bool))
			{
				data = NULL;
				return;
			}

			// Get the data.
			data = new bool((0 == buffer[0]) ? false : true);
	    }

		BoolDatatype::BoolDatatype(const BoolDatatype & source)
		{
			data = new bool(*source.data);
		}

		BoolDatatype::~BoolDatatype(void )
		{
			delete data;
		}

		BoolDatatype & BoolDatatype::operator =(const BoolDatatype & source)
		{
			delete data;
			data = new bool(*source.data);
			return *this;
		}

		void *BoolDatatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool BoolDatatype::operator ==(Datatype * check) const
		{
			BoolDatatype * msg = dynamic_cast<BoolDatatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

	    std::size_t BoolDatatype::GetFlattenSize(void) const
	    {
	    	return sizeof(bool);
	    }

	    Datatype *BoolDatatype::Duplicate(void) const
		{
	    	return new BoolDatatype(*data);
		}

	    std::size_t BoolDatatype::Flatten(uint8 *buffer, std::size_t size) const
	    {
	    	// Check the buffer and size.
	    	if (NULL == buffer || size < sizeof(bool))
	    	{
	    		// The buffer is not big enough, so don't copy anything.
				return 0;
	    	}
	    	else
	    	{
	    		// Copy the data.
	    		buffer[0] = (uint8)(*data ? 1 : 0);
				return sizeof(bool);
	    	}
	    }
	}
}
