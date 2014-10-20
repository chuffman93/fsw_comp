/*! \file DoubleDatatype.h
 *  \brief Header File for the DoubleDatatype Class.
 *
 *  This class defines a wrapper class for a double-precision floating point
 *  value.
 */

#ifndef _DOUBLEDATATYPE_H_
#define _DOUBLEDATATYPE_H_

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace Phoenix
{
	namespace Core
	{
		/*! \brief Wrapper for a Double-Precision Floating Point Value
		 *
		 *  This class wraps a double-precision floating point value and
		 *  adheres to the interface set by the Datatype class.
		 */
		class DoubleDatatype: public Datatype
		{
		public:
			/*! \brief Constructor for DoubleDatatype.
			 *
			 *  Sets the data to 0.0.
			 */
			DoubleDatatype(void );

			/*! \brief Constructor for DoubleDatatype
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn Data to be copied.
			 */
			DoubleDatatype(double dataIn);

			/*! \brief Constructor for DoubleDatatype
			 *
			 *  Unpacks the double data from a buffer.  The buffer should
			 *  consist of 8 bytes that contain the double data as
			 *  it is stored by a big-Endian system.
			 *
			 *  \param buffer Buffer containing the Flattened data.
			 *  \param size Size of buffer.
			 */
			DoubleDatatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for DoubleDatatype */
			DoubleDatatype(const DoubleDatatype & source);

			/*! \brief Destructor for DoubleDatatype */
			virtual ~DoubleDatatype(void );

			/*! \brief Assignment Operator for DoubleDatatype */
			DoubleDatatype & operator =(const DoubleDatatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */

			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			double * data;
		};
	}
}

#endif /* _DOUBLEDATATYPE_H_ */
