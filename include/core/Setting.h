/*! \file Setting.h
 *  \brief Header File for the Setting Class.
 *
 *  This file defines the Setting class, which represents a
 *  configuration setting.
 *
 */

#ifndef _SETTING_H
#define _SETTING_H

#include "core/StdTypes.h"
#include "core/VariableTypeData.h"

namespace AllStar
{
    namespace Core
    {
		/*! \brief Setting class for all configuration data.
		 *
		 *  This file defines the Setting class, which represents a
		 *  configuration setting.
		 */
        class Setting
        {
        public:
        	/*! \brief Default Constructor for Setting.
			 *
			 *  \note Initializes the item to 0, and uses the default constructor
			 *  for data
			 */
        	Setting(void );

        	/*! \brief Constructor for Setting from ConfigItemType and VariableTypeData.
			 *
			 *  \param itemIn Config item of the setting.
			 *  \param valueIn VariableTypeData of the setting.
			 */
            Setting(ConfigItemType itemIn, const VariableTypeData & valueIn);

            /*! \brief Constructor for Setting from a Buffer.
			 *
			 *  \param buffer Buffer input
			 *  \param size Size of the buffer
			 */
            Setting(uint8 * buffer, std::size_t size);

            /*! \brief Copy Constructor for Setting
			 *
			 *  \param source Object to be copied.
			 */
            Setting(const Setting & source);

            /*! \brief Destructor for Setting. */
            virtual ~Setting(void );

            /*! \brief Assignment Operator for Setting.
			 *
			 *  \param source Object to be copied.
			 *  \return Reference to the calling object.
			 */
            Setting & operator=(const Setting & source);

            /*! \brief Returns the item of this message.
			 *
			 *  \return MessageConfigItem of this message.
			 *
			 */
            ConfigItemType GetItem(void ) const;

            /*! \brief Returns the value of this message.
			 *
			 *  \return VariableTypeData of this message.
			 *
			 */
            VariableTypeData & GetValue(void );

            /*! \brief Returns the minimum buffer size required by Flatten( ).
			 *
			 *  \return Minimum Flatten( ) buffer size.
			 *
			 */
            virtual std::size_t GetFlattenSize(void ) const;

            /*! \brief Flattens the calling object into the given buffer.
			 *
			 *  \param buffer Buffer to copy data into.
			 *  \param size Size of buffer.
			 *
			 *  \return Number of bytes actually copied into buffer.
			 *
			 */
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size);

            /*! \brief Equals Operator for Setting.
			 *
			 *  \param check Setting object to compare to.
			 *
			 *  \return Whether this object is equal to the object
			 *  pointed to by check.
			 *
			 */
            virtual bool operator ==(const Setting & check) const;
        private:
            /*! \brief Item of the message. */
            ConfigItemType item;
            /*! \brief Value of the message. */
            VariableTypeData value;
        };
    }
}

#endif  //_SETTING_H
