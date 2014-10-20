/*! \file Int32Datatype.h
 *  \brief Header File for the Int32Datatype Class.
 *
 *  This class defines a wrapper class for a 32-bit signed integer value.
 */

#ifndef _INT32DATATYPE_H_
#define _INT32DATATYPE_H_

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace Phoenix
{
	namespace Core
	{
		/*! \brief Wrapper for a 32-Bit Signed Integer Value
		 *
		 *  This class wraps a 32-bit signed integer value and adheres to the
		 *  interface set by the Datatype class.
		 */
		class Int32Datatype: public Datatype
		{
		public:
			/*! \brief Constructor for Int32Datatype.
			 *
			 *  Sets the data to false.
			 */
			Int32Datatype(void );

			/*! \brief Constructor for Int32Datatype
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn Data to be copied.
			 */
			Int32Datatype(int32 dataIn);

			/*! \brief Constructor for Int32Datatype
			 *
			 *  Unpacks the integer data from a buffer.  The buffer should
			 *  consist of four bytes packed in a big-Endian manner.
			 *
			 *  \param buffer Buffer containing the Flattened data.
			 *  \param size Size of buffer.
			 */
			Int32Datatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for Int32Datatype */
			Int32Datatype(const Int32Datatype & source);

			/*! \brief Destructor for Int32Datatype */
			virtual ~Int32Datatype(void );

			/*! \brief Assignment Operator for Int32Datatype */
			Int32Datatype & operator =(const Int32Datatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */

			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			int32 * data;
		};
	}
}

#endif /* _INT32DATATYPE_H_ */
