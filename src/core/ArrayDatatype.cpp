/*! \file ArrayDatatype.cpp
 *  \brief Implementation File for the ArrayDatatype Class.
 *
 *  This class defines a wrapper class for a byte array.
 */

#include "core/ArrayDatatype.h"

using namespace std;

namespace AllStar
{
    namespace Core
    {
        ArrayDatatype::ArrayDatatype(void )
        : data(new vector<uint8>( ))
        {
            // Left Intentionally Empty
        }

        ArrayDatatype::ArrayDatatype(std::vector<uint8> dataIn)
        : data(new vector<uint8>(dataIn))
        {
            // Left Intentionally Empty
        }

        ArrayDatatype::ArrayDatatype(uint8 * buffer, std::size_t size)
        {
            size_t dataSize;

            // Check the buffer and size.
            if (NULL == buffer || size < sizeof(uint32))
            {
                data = NULL;
                return;
            }

            // Get the size of the array.
            dataSize = (buffer[0] << 24) | (buffer[1] << 16) |
                       (buffer[2] << 8)  | (buffer[3]);
            size -= sizeof(uint32);
            buffer += sizeof(uint32);

            // Check that the size and dataSize match.
            if (size < dataSize)
            {
                // They don't match, so set data to NULL.
                data = NULL;
            }
            else
            {
                // It's a match!  Create a new array from the buffer.
                data = new vector<uint8>(buffer, buffer + dataSize);
            }
        }

        ArrayDatatype::ArrayDatatype(const ArrayDatatype & source)
        {
            // Copy the data in source.
            data = new vector<uint8>(*source.data);
        }

        ArrayDatatype::~ArrayDatatype(void)
        {
            delete data;
        }

        ArrayDatatype & ArrayDatatype::operator =(const ArrayDatatype & source)
        {
            delete data;
            data = new vector<uint8>(*source.data);
            return *this;
        }

        void * ArrayDatatype::GetDataPtr(void) const
        {
            return (void *)data;
        }

        bool ArrayDatatype::operator ==(Datatype * check) const
		        {
            ArrayDatatype * msg = dynamic_cast<ArrayDatatype *>(check);
            if (NULL == msg)
            {
                return false;
            }
            else
            {
                return *data == *msg->data;
            }
		        }

        std::size_t ArrayDatatype::GetFlattenSize(void) const
        {
            return data->size( )+sizeof(uint32);
        }

        Datatype *ArrayDatatype::Duplicate(void) const
        {
            return new ArrayDatatype(*data);
        }

        std::size_t ArrayDatatype::Flatten(uint8 *buffer, std::size_t size) const
        {
            // Check the buffer and size.
            if (NULL == buffer || size < GetFlattenSize( ))
            {
                // The buffer is not big enough, so don't copy anything.
                return 0;
            }
            else
            {
                // Copy in the size.
                size_t dataSize = data->size( );
                buffer[0] = (dataSize >> 24) & 0xff;
                buffer[1] = (dataSize >> 16) & 0xff;
                buffer[2] = (dataSize >> 8) & 0xff;
                buffer[3] = (dataSize) & 0xff;
                buffer += sizeof(uint32);

                // Copy the data;
                for (vector<uint8>::iterator it = data->begin( ); it != data->end( ); ++it)
                {
                    *(buffer++) = *it;
                }
                return GetFlattenSize( );
            }
        }
    }
}
