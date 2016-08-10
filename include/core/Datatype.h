/*! \file Datatype.h
 *  \brief Header File for the Datatype Class.
 *
 *  This class defines an abstract class that defines a generic data type
 *  with a consistent interface.
 */

#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include "core/StdTypes.h"

namespace AllStar
{
    namespace Core
    {
        /*! \brief Abstraction of a Data Type.
         *
         *  This abstract class defines a standard interface for subclasses
         *  that can wrap a given data type.
         */
        class Datatype
        {
        public:
            /*! \brief Destructor for Datatype. */
            virtual ~Datatype(void ) = 0;

            /*! \brief Returns a pointer to the data.
             *
             *  \return A pointer to the data.
             *
             *  \note When using this function, always be sure to cast the
             *  pointer correctly based on the concrete Datatype class
             *  being used.
             */
            virtual void * GetDataPtr(void ) const = 0;

            /*! \brief Equals Operator for Datatype.
             *
             *  \param check Pointer to Datatype object to compare
             *  \return true if this object is equal to *check.
             */
            virtual bool operator ==(Datatype * check) const = 0;

            /*! \brief Returns the minimum buffer size required by Flatten( ).
             *
             *  \return Minimum Flatten( ) buffer size.
             */
            virtual std::size_t GetFlattenSize(void ) const = 0;

            /*! \brief Duplicates the Datatype object.
             *
             *  \return A copy of the calling object allocated on the heap.
             *
             *  \note Each subclass of Datatype should overload this method by
             *  returning new [ConcreteSubclassName](*this).
             *
             *  \note The returned pointer should be deleted by the calling
             *  program when it is no longer needed.
             */
            virtual Datatype * Duplicate(void ) const = 0;

            /*! \brief Flattens the calling object into the given buffer.
             *
             *  \param buffer Buffer to copy data into.
             *  \param size Size of buffer.
             *  \return Number of bytes actually copied into buffer.
             */
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const = 0;
        };
    }
}

#endif /* DATATYPE_H_ */
