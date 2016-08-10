/*! \file ArrayDatatype.h
 *  \brief Header File for the ArrayDatatype Class.
 *
 *  This class defines a wrapper class for a byte array.
 */

#ifndef _ARRAYDATATYPE_H_
#define _ARRAYDATATYPE_H_

#include <vector>

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace AllStar
{
    namespace Core
    {
        /*! \brief Wrapper for a Byte Array
         *
         *  This class wraps a STL vector of uint8 values (bytes) and adheres
         *  to the interface set by the Datatype class.
         */
        class ArrayDatatype: public Datatype
        {
        public:
            /*! \brief Constructor for ArrayDatatype
             *
             *  Sets the data to an empty vector.
             */
            ArrayDatatype(void );

            /*! \brief Constructor for ArrayDatatype
             *
             *  Sets the data to a copy of dataIn.
             *
             *  \param dataIn Array to be copied.
             */
            ArrayDatatype(std::vector<uint8> dataIn);

            /*! \brief Constructor for ArrayDatatype
             *
             *  Unpacks the array from a buffer.  The buffer must have 4 bytes
             *  that indicate the length of the array (packed in a big-Endian
             *  manner) followed by the array data.
             *
             *  \param buffer Buffer containing the Flattened array.
             *  \param size Size of buffer.
             */
            ArrayDatatype(uint8 * buffer, std::size_t size);

            /*! \brief Copy Constructor for ArrayDatatype */
            ArrayDatatype(const ArrayDatatype & source);

            /*! \brief Destructor for ArrayDatatype */
            virtual ~ArrayDatatype(void );

            /*! \brief Assignment Operator for ArrayDatatype */
            ArrayDatatype & operator =(const ArrayDatatype & source);

            /* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */
            virtual void * GetDataPtr(void ) const;

            virtual bool operator ==(Datatype * check) const;

            virtual std::size_t GetFlattenSize(void ) const;

            virtual Datatype * Duplicate(void ) const;

            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

        private:
            std::vector<uint8> * data; //!< Pointer to the array.
        };
    }
}

#endif /* _ARRAYDATATYPE_H_ */
