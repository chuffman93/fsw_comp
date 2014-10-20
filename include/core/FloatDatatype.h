/*! \file FloatDatatype.h
 *  \brief Header File for the FloatDatatype Class.
 *
 *  This class defines a wrapper class for a single-precision floating point
 *  value.
 */

#ifndef _FLOATDATATYPE_H_
#define _FLOATDATATYPE_H_

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace Phoenix
{
	namespace Core
	{
		/*! \brief Wrapper for a Single-Precision Floating Point Value
		 *
		 *  This class wraps a single-precision floating point value and
		 *  adheres to the interface set by the Datatype class.
		 */
		class FloatDatatype: public Datatype
		{
		public:
			/*! \brief Constructor for FloatDatatype.
			 *
			 *  Sets the data to 0.0.
			 */
			FloatDatatype(void );

			/*! \brief Constructor for FloatDatatype
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn Data to be copied.
			 */
			FloatDatatype(float dataIn);

			/*! \brief Constructor for FloatDatatype
			 *
			 *  Unpacks the float data from a buffer.  The buffer should
			 *  consist of 8 bytes that contain the float data as
			 *  it is stored by a big-Endian system.
			 *
			 *  \param buffer Buffer containing the Flattened data.
			 *  \param size Size of buffer.
			 */
			FloatDatatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for FloatDatatype */
			FloatDatatype(const FloatDatatype & source);\

			/*! \brief Destructor for FloatDatatype */
			virtual ~FloatDatatype(void );

			/*! \brief Assignment Operator for FloatDatatype */
			FloatDatatype & operator =(const FloatDatatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */

			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			float * data;
		};
	}
}

#endif /* _FLOATDATATYPE_H_ */
