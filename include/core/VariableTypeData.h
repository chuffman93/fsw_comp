/*! \file VariableTypeData.h
 *  \brief Header File for the VariableTypeData Class.
 *
 *  This file defines the VariableTypeData class, which represents data
 *  with an arbitrary type.
 */


#ifndef _VARIABLETYPEDATA_H
#define _VARIABLETYPEDATA_H

#include "core/Datatype.h"
#include "core/VariableTypeEnum.h"
#include "core/FSWPacket.h"
#include "core/StdTypes.h"

#include <string>
#include <vector>

namespace Phoenix
{
    namespace Core
    {
		/*! \brief Abstraction of Data with an Arbitrary Type.
		 *
		 *  This class defines a standard data type that abstracts the type of value
		 *  from the interface, allowing functions that would normally return
		 *  different types to return this class instead.
		 */
        class VariableTypeData
        {
        public:
        	/*! \brief Default Constructor for VariableTypeData.
        	 *
        	 *  \note Initializes the type to Void and the data pointer
        	 *  to NULL.
        	 */
        	VariableTypeData(void );

        	/*! \brief Constructor for VariableTypeData from an Integer.
        	 *
        	 *  \param dataIn Signed integer data.
        	 */
        	VariableTypeData(int32 dataIn);

        	/*! \brief Constructor for VariableTypeData from an Unsigned
        	 *  Integer.
			 *
			 *  \param dataIn Unsigned integer data.
			 */
        	VariableTypeData(uint32 dataIn);

        	/*! \brief Constructor for VariableTypeData from a Boolean.
			 *
			 *  \param dataIn Boolean data.
			 */
        	VariableTypeData(bool dataIn);

        	/*! \brief Constructor for VariableTypeData from a Float
			 *
			 *  \param dataIn Float data.
			 */
        	VariableTypeData(float dataIn);

        	/*! \brief Constructor for VariableTypeData from a Double.
			 *
			 *  \param dataIn Double data.
			 */
        	VariableTypeData(double dataIn);

        	/*! \brief Constructor for VariableTypeData from a String.
			 *
			 *  \param dataIn String data.
			 */
        	VariableTypeData(std::string dataIn);
			

        	/*! \brief Constructor for VariableTypeData from a C-String.
			 *
			 *  \param dataIn String data.
			 */
			VariableTypeData(char * dataIn);

        	VariableTypeData(const std::vector<uint8> & dataIn);
			
			/*! \brief Constructor for VariableTypeData from a FSWPacket.
			 *
			 *  \param dataIn FSWPacket data.
			 */
        	VariableTypeData(FSWPacket dataIn);

        	/*! \brief Constructor for VariableTypeData from a Buffer.
        	 *
        	 *  \param buffer Buffer input
        	 *  \param size Size of the buffer
        	 */
            VariableTypeData(uint8 * buffer, std::size_t size);

            /*! \brief Copy Constructor for VariableTypeData.
             *
             *  \param source Object to be copied.
             */
            VariableTypeData(const VariableTypeData & source);

            /*! \brief Destructor for VariableTypeData. */
            virtual ~VariableTypeData(void );

            /*! \brief Assignment Operator for VariableTypeData.
             *
             *  \param source Object to be copied.
             *  \return Reference to the calling object.
             */
            VariableTypeData & operator=(const VariableTypeData & source);

            /*! \brief Returns the type of the data.
             *
             *  \return The type of the data.
             */
            VariableTypeEnum GetType(void ) const;

            /*! \brief Returns a pointer to the data.
             *
             *  \return A pointer to the data.
             *
             *  \note When using this function, always be sure to cast the
             *  pointer correctly based on the return value of
             *  GetType( ).
             */
            void * GetData(void ) const;

            /*! \brief Equals Operator for VariableTypeData
			 *
			 *  \param check VariableTypeData object to compare
			 *  \return true if this object is equal to check.
			 */
			bool operator ==(const VariableTypeData & check) const;

            /*! \brief Returns the minimum buffer size required by Flatten( ).
             *
             *  \return Minimum Flatten( ) buffer size.
             */
            virtual std::size_t GetFlattenSize(void ) const;

            /*! \brief Flattens the calling object into the given buffer.
             *
             *  \param buffer Buffer to copy data into.
             *  \param size Size of buffer.
             *  \return Number of bytes actually copied into buffer.
             */
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;
        private:
            /*! \brief Type of the data. */
            VariableTypeEnum type;

            /*! Pointer to the data representation. */
            Datatype * data;
        };
    }
}

#endif  //_VARIABLETYPEDATA_H
