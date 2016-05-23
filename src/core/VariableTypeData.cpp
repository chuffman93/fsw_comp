/*! \file VariableTypeData.cpp
 *  \brief Implementation File for the VariableTypeData Class.
 *
 *  This file defines the VariableTypeData class, which represents data
 *  with an arbitrary type.
 */

#include "core/VariableTypeData.h"
#include "core/Int32Datatype.h"
#include "core/Uint32Datatype.h"
#include "core/BoolDatatype.h"
#include "core/FloatDatatype.h"
#include "core/DoubleDatatype.h"
#include "core/StringDatatype.h"
#include "core/ArrayDatatype.h"
#include "core/PacketDatatype.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
		VariableTypeData::VariableTypeData(void )
		                 : type(VAR_TYPE_ENUM_VOID), data(NULL)
        {
			// Left Intentionally Empty
        }

		VariableTypeData::VariableTypeData(int32 dataIn)
		                : type(VAR_TYPE_ENUM_INT)
		{
			data = new Int32Datatype(dataIn);
		}

		VariableTypeData::VariableTypeData(uint32 dataIn)
		                 : type(VAR_TYPE_ENUM_UNSIGNED_INT)
		{
			data = new Uint32Datatype(dataIn);
		}

		VariableTypeData::VariableTypeData(bool dataIn)
				         : type(VAR_TYPE_ENUM_BOOL)
		{
			data = new BoolDatatype(dataIn);
		}

		VariableTypeData::VariableTypeData(float dataIn)
						 : type(VAR_TYPE_ENUM_FLOAT)
		{
			data = new FloatDatatype(dataIn);
		}

		VariableTypeData::VariableTypeData(double dataIn)
						 : type(VAR_TYPE_ENUM_DOUBLE)
		{
			data = new DoubleDatatype(dataIn);
		}

		VariableTypeData::VariableTypeData(std::string dataIn)
						 : type(VAR_TYPE_ENUM_STRING)
		{
			data = new StringDatatype(dataIn);
		}

		VariableTypeData::VariableTypeData(char * dataIn)
		                 : type(VAR_TYPE_ENUM_STRING)
		{
			data = new StringDatatype(string(dataIn));
		}

		VariableTypeData::VariableTypeData(const std::vector<uint8> & dataIn)
						 : type(VAR_TYPE_ENUM_ARRAY)
		{
			data = new ArrayDatatype(dataIn);
		}
		
		VariableTypeData::VariableTypeData(FSWPacket dataIn)
						 : type(VAR_TYPE_ENUM_PACKET)
		{
			data = new PacketDatatype(dataIn);
		}

        VariableTypeData::VariableTypeData(uint8 * buffer, std::size_t size)
        {
        	// Check that the buffer is sized correctly.
        	if (NULL == buffer || size-- < 1)
        	{
        		type = VAR_TYPE_ENUM_VOID;
        		data = NULL;
        		return;
        	}
        	// Get the type.
        	type = (VariableTypeEnum)(*(buffer++));

        	// Construct the data member depending on the type.
        	switch (type)
			{
			case VAR_TYPE_ENUM_INT:
				data = new Int32Datatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_UNSIGNED_INT:
				data = new Uint32Datatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_BOOL:
				data = new BoolDatatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_FLOAT:
				data = new FloatDatatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_DOUBLE:
				data = new DoubleDatatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_STRING:
				data = new StringDatatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_ARRAY:
				data = new ArrayDatatype(buffer, size);
				break;
			case VAR_TYPE_ENUM_PACKET:
				data = new PacketDatatype(buffer, size);
				break;
			default:
				type = VAR_TYPE_ENUM_VOID;
				data = NULL;
				return;
			}

        	// Check that the allocation worked correctly.
        	if (NULL == data || NULL == data->GetDataPtr( ))
        	{
        		type = VAR_TYPE_ENUM_VOID;
        		//fixme this is fucked
        		delete data;
        		data = NULL;
        	}
        }
        
        VariableTypeData::VariableTypeData(const VariableTypeData & source)
        {
        	// Copy the type.
        	type = source.type;

        	// Copy the data.
        	if (VAR_TYPE_ENUM_VOID == type)
        	{
        		data = NULL;
        	}
        	else if (source.data == NULL)
        	{
        		type = VAR_TYPE_ENUM_VOID;
        		data = NULL;
        	}
        	else
        	{
        		data = source.data->Duplicate( );
        	}
        }
        
        VariableTypeData::~VariableTypeData(void )
        {
        	delete data;
        }
        
        VariableTypeData & VariableTypeData::operator=(const VariableTypeData & source)
        {
        	delete data;
        	type = source.type;
        	if (VAR_TYPE_ENUM_VOID == type)
        	{
        		data = NULL;
        	}
        	else if (source.data == NULL)
        	{
        		type = VAR_TYPE_ENUM_VOID;
        		data = NULL;
        	}
        	else
        	{
        		data = source.data->Duplicate( );
        	}
        	return *this;
        }
        
        VariableTypeEnum VariableTypeData::GetType(void ) const
        {
        	return type;
        }
        
        void * VariableTypeData::GetData(void ) const
        {
        	if (data != NULL)
        	{
        		return data->GetDataPtr( );
        	}
        	else
        	{
        		return NULL;
        	}
        }
        
        bool VariableTypeData::operator ==(const VariableTypeData & check) const
		{
        	if (type == check.type)
        	{
        		if (data != NULL && check.data != NULL)
				{
					return *data == check.data;
				}
        		else if (data == NULL && check.data == NULL)
        		{
        			return true;
        		}
        		else
        		{
        			return false;
        		}
        	}
        	else
        	{
        		return false;
        	}
		}

        std::size_t VariableTypeData::GetFlattenSize(void ) const
        {
        	// Size of type field.
        	size_t size = sizeof(uint8);

        	// Size of data field.
        	if (data != NULL)
        	{
        		size += data->GetFlattenSize( );
        	}

        	return size;
        }
        
        std::size_t VariableTypeData::Flatten(uint8 * buffer, std::size_t size) const
        {
        	size_t numCopied = 0;

        	// Check the buffer and buffer size.
        	if (NULL == buffer || size < GetFlattenSize( ))
        	{
        		return 0;
        	}

        	// Copy the type.
        	*(buffer++) = type;
        	++numCopied;
        	--size;

        	// Check that data is not NULL.
        	if (NULL == data)
        	{
        		return numCopied;
        	}

        	// Copy the data.
        	numCopied += data->Flatten(buffer, size);

        	return numCopied;
        }
    }
}
