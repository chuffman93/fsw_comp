/*! \file StringDatatype.h
 *  \brief Header File for the StringDatatype Class.
 *
 *  This class defines a wrapper class for a string.
 */

#ifndef _STRINGDATATYPE_H_
#define _STRINGDATATYPE_H_

#include <string>

#include "core/StdTypes.h"
#include "core/Datatype.h"

namespace AllStar
{
	namespace Core
	{
		/*! \brief Wrapper for a String
		 *
		 *  This class wraps a STL string and adheres to the interface set by
		 *  the Datatype class.
		 */
		class StringDatatype: public Datatype
		{
		public:
			/*! \brief Constructor for StringDatatype
			 *
			 *  Sets the data to an empty string.
			 */
			StringDatatype(void );

			/*! \brief Constructor for StringDatatype
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn String to be copied.
			 */
			StringDatatype(std::string dataIn);

			/*! \brief Constructor for StringDatatype
			 *
			 *  Unpacks the string from a buffer.  The buffer must have 4 bytes
			 *  that indicate the length of the string (packed in a big-Endian
			 *  manner) followed by the string data.
			 *
			 *  \param buffer Buffer containing the Flattened string.
			 *  \param size Size of buffer.
			 */
			StringDatatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for StringDatatype */
			StringDatatype(const StringDatatype & source);

			/*! \brief Destructor for StringDatatype */
			virtual ~StringDatatype(void );

			/*! \brief Assignment Operator for StringDatatype */
			StringDatatype & operator =(const StringDatatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */

			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			std::string * data;
		};
	}
}

#endif /* _STRINGDATATYPE_H_ */
