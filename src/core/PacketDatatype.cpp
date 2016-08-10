/*
 * PacketDatatype.cpp
 *
 * Created: 7/25/2013 3:56:24 PM
 *  Author: Admin
 */

#include "core/PacketDatatype.h"
#include "core/FSWPacket.h"

using namespace std;

namespace AllStar
{
	namespace Core
	{
		PacketDatatype::PacketDatatype(void )
		: data(new FSWPacket())
		{
			// Left Intentionally Empty
		}

		PacketDatatype::PacketDatatype(FSWPacket dataIn)
		: data(new FSWPacket(dataIn))
		{
			// Left Intentionally Empty
		}

		PacketDatatype::PacketDatatype(uint8 * buffer, std::size_t size)
		{
			//cout << "PacketDatatype constructor starting. size:" << size << endl;
			// Check the buffer and the size.
			if (NULL == buffer || size < (sizeof(LocationIDType) + sizeof(LocationIDType) + sizeof(uint16) + sizeof(uint32)))
			{
				data = NULL;
				return;
			}

			data = new FSWPacket(buffer, size);

			if(data == NULL || data->GetSource() == LOCATION_ID_INVALID || data->GetDestination() == LOCATION_ID_INVALID || data->GetMessageBufPtr() == NULL)
			{
				if(data != NULL)
				{
					delete data;
				}
				data = NULL;
			}
		}

		PacketDatatype::PacketDatatype(const PacketDatatype & source)
		{
			data = new FSWPacket(*source.data);
		}

		PacketDatatype::~PacketDatatype(void )
		{
			delete data;
		}

		PacketDatatype & PacketDatatype::operator =(const PacketDatatype & source)
		{
			delete data;
			data = new FSWPacket(*source.data);
			return *this;
		}

		void * PacketDatatype::GetDataPtr(void) const
		{
			return (void *)data;
		}

		bool PacketDatatype::operator ==(Datatype * check) const
		{
			PacketDatatype * msg = dynamic_cast<PacketDatatype *>(check);
			if (NULL == msg)
			{
				return false;
			}
			else
			{
				return *data == *msg->data;
			}
		}

		std::size_t PacketDatatype::GetFlattenSize(void) const
		{
			return data->GetFlattenSize();
		}

		Datatype *PacketDatatype::Duplicate(void) const
		{
			return new PacketDatatype(*data);
		}

		std::size_t PacketDatatype::Flatten(uint8 *buffer, std::size_t size) const
		{
			// Check the buffer and the size.
			if (NULL == buffer || size < data->GetFlattenSize())
			{
				// The buffer is not big enough, so don't copy anything.
				return 0;
			}
			else
			{
				// Flatten data.
				if(data->Flatten(buffer, size) == 0)
				{
					return 0;
				}

				return data->GetFlattenSize();
			}
		}
	}
}
