/*! \file Setting.cpp
 *  \brief Implementation File for the Setting Class.
 *
 *  This file defines the Setting class, which represents a
 *  configuration setting.
 */

#include "core/Setting.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
		Setting::Setting()
				:item(0), value()
		{
			// Intentionally left empty
		}
        Setting::Setting(ConfigItemType itemIn, const VariableTypeData & valueIn)
				:item(itemIn), value(valueIn)
        {
        	// Intentionally left empty
        }

        Setting::Setting(uint8 * buffer, size_t size)
        {
        	// Check if the buffer is null or size is too small
        	if (NULL == buffer || size < GetFlattenSize())
			{
				item = 0;
				value = VariableTypeData();
				return;
			}

        	// Get the item from the buffer (4 bytes)
           	item = *buffer++;
        	item = item << 8;

        	item |= *buffer++;
        	item = item << 8;

        	item |= *buffer++;
        	item = item << 8;

        	item |= *buffer++;

        	// Update the size
			size -= sizeof(ConfigItemType);

			// Create value from the buffer
			value = VariableTypeData(buffer, size);
        }

        Setting::Setting(const Setting & source)
				: item(source.item), value(source.value)
        {
        	// Left intentionally empty
        }

        Setting::~Setting(void )
        {
        	// Left intentionally empty
        }

        Setting & Setting::operator=(const Setting & source)
        {
        	// Make sure this and source aren't the same
        	if (this  == &source)
			{
				return *this;
			}

			item = source.item;

			value = source.value;

			return *this;

        }

        ConfigItemType Setting::GetItem(void ) const
        {
        	return item;
        }

        VariableTypeData & Setting::GetValue(void )
        {
        	return value;
        }

        size_t Setting::GetFlattenSize(void ) const
        {
        	// Size of item
        	size_t size = sizeof(ConfigItemType);

        	// Size of value
        	size += value.GetFlattenSize();

        	return size;
        }

        size_t Setting::Flatten(uint8 * buffer, size_t size)
		{
        	size_t numCopied = 0;

			// Check the buffer and buffer size.
			if (NULL == buffer || size < GetFlattenSize())
			{
				return 0;
			}

			// Put the item in the buffer (4 bytes)
        	*buffer++ = (item >> 24) & 0xFF;
        	*buffer++ = (item >> 16) & 0xFF;
        	*buffer++ = (item >> 8) & 0xFF;
        	*buffer++ = item & 0xFF;

        	// Update the size and numCopied
        	size -= sizeof(ConfigItemType);
        	numCopied += sizeof(ConfigItemType);

        	// Put the value into the buffer and update numCopied
        	numCopied += value.Flatten(buffer, size);

        	return numCopied;
        }

        bool Setting::operator ==(const Setting & check) const
		{
        	// Check that items and values are equal
        	return ((item == check.item) &&
        			(value == check.value));
		}
    }
}
