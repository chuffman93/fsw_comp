/*! \file Uint32Datatype.h
 *  \brief Header File for the Uint32Datatype Class.
 *
 *  This class defines a wrapper class for a 32-bit unsigned integer value.
 */

#ifndef _UINT32DATATYPE_H_
#define _UINT32DATATYPE_H_

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace AllStar
{
	namespace Core
	{
		/*! \brief Wrapper for a 32-Bit Unsigned Integer Value
		 *
		 *  This class wraps a 32-bit unsigned integer value and adheres to the
		 *  interface set by the Datatype class.
		 */
		class Uint32Datatype: public Datatype
		{
		public:
			/*! \brief Constructor for Uint32Datatype.
			 *
			 *  Sets the data to false.
			 */
			Uint32Datatype(void );

			/*! \brief Constructor for Uint32Datatype
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn Data to be copied.
			 */
			Uint32Datatype(uint32 dataIn);

			/*! \brief Constructor for Uint32Datatype
			 *
			 *  Unpacks the integer data from a buffer.  The buffer should
			 *  consist of four bytes packed in a big-Endian manner.
			 *
			 *  \param buffer Buffer containing the Flattened data.
			 *  \param size Size of buffer.
			 */
			Uint32Datatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for Uint32Datatype */
			Uint32Datatype(const Uint32Datatype & source);

			/*! \brief Destructor for Uint32Datatype */
			virtual ~Uint32Datatype(void );

			/*! \brief Assignment Operator for Uint32Datatype */
			Uint32Datatype & operator =(const Uint32Datatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */

			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			uint32 * data;
		};
	}
}

#endif /* _UINT32DATATYPE_H_ */
