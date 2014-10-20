/*! \file BoolDatatype.h
 *  \brief Header File for the BoolDatatype Class.
 *
 *  This class defines a wrapper class for a Boolean value.
 */

#ifndef _BOOLDATATYPE_H_
#define _BOOLDATATYPE_H_

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace Phoenix
{
	namespace Core
	{
		/*! \brief Wrapper for a Boolean Value
		 *
		 *  This class wraps a Boolean value and adheres to the interface set
		 *  by the Datatype class.
		 */
		class BoolDatatype: public Datatype
		{
		public:
			/*! \brief Constructor for BoolDatatype.
			 *
			 *  Sets the data to false.
			 */
			BoolDatatype(void );

			/*! \brief Constructor for BoolDatatype
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn Data to be copied.
			 */
			BoolDatatype(bool dataIn);

			/*! \brief Constructor for BoolDatatype
			 *
			 *  Unpacks the Boolean data from a buffer.  The buffer should
			 *  consist of a single byte that is 0 if the data should be
			 *  false and 1 if the data should be true.
			 *
			 *  \param buffer Buffer containing the Flattened data.
			 *  \param size Size of buffer.
			 */
			BoolDatatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for BoolDatatype */
			BoolDatatype(const BoolDatatype & source);

			/*! \brief Destructor for BoolDatatype */
			virtual ~BoolDatatype(void );

			/*! \brief Assignment Operator for BoolDatatype */
			BoolDatatype & operator =(const BoolDatatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */
			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			bool * data; //<! Pointer to the data.
		};
	}
}

#endif /* _BOOLDATATYPE_H_ */
