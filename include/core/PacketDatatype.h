/*
 * PacketDatatype.h
 *
 * Created: 7/25/2013 3:48:44 PM
 *  Author: Admin
 */ 


#ifndef PACKETDATATYPE_H_
#define PACKETDATATYPE_H_

#include "core/StdTypes.h"
#include "core/Datatype.h"
#include "core/FSWPacket.h"

namespace AllStar
{
	namespace Core
	{
		/*! \brief Wrapper for a FSWPacket
		 *
		 *  This class wraps a FSWPacket and
		 *  adheres to the interface set by the Datatype class.
		 */
		class PacketDatatype: public Datatype
		{
		public:
			/*! \brief Constructor for FSWPacket.
			 *
			 *  Sets the data to the default constructor for a FSWPacket.
			 */
			PacketDatatype(void );

			/*! \brief Constructor for FSWPacket
			 *
			 *  Sets the data to a copy of dataIn.
			 *
			 *  \param dataIn Data to be copied.
			 */
			PacketDatatype(FSWPacket dataIn);

			/*! \brief Constructor for FSWPacket
			 *
			 *  Unpacks the FSWPacket data from a buffer.  Creates a packet
			 *  object from the given buffer as specified in the 
			 *  xxx Specification.
			 *
			 *  \param buffer Buffer containing the Flattened data.
			 *  \param size Size of buffer.
			 */
			PacketDatatype(uint8 * buffer, std::size_t size);

			/*! \brief Copy Constructor for PacketDatatype */
			PacketDatatype(const PacketDatatype & source);

			/*! \brief Destructor for PacketDatatype */
			virtual ~PacketDatatype(void );

			/*! \brief Assignment Operator for PacketDatatype */
			PacketDatatype & operator =(const PacketDatatype & source);

			/* ALL FUNCTIONS BELOW DOCUMENTED IN DATATYPE.H */

			virtual void * GetDataPtr(void ) const;

			virtual bool operator ==(Datatype * check) const;

			virtual std::size_t GetFlattenSize(void ) const;

			virtual Datatype * Duplicate(void ) const;

			virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

		private:
			FSWPacket * data;
		};
	}
}

#endif /* PACKETDATATYPE_H_ */