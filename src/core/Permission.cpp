/*! \file Permission.h
 *  \brief Implementation File for the Permission Class.
 *
 *  This class encapsulates the permissions for each server to perform some action.
 */

#include "core/Permission.h"

namespace AllStar
{
    namespace Core
    {
		//serverPermissions() constructor sets all bits to false
        Permission::Permission(bool allSet)
				   :serverPermissions()
        {
        	//Default value of allSet if none is provided is to be false.
        	if (allSet)
        	{
        		//Sets all bits in the bitset
        		serverPermissions.set();
        	}
        }
        
        Permission::Permission(const Permission & source)
				   :serverPermissions(source.serverPermissions)
        {
        	// Intentionally left empty
        }
        
        Permission::~Permission(void )
        {
        	// Intentionally left empty
        }
        
        Permission & Permission::operator=(const Permission & source)
        {
        	if (this == &source)
        	{
        		return *this;
        	}

        	serverPermissions = source.serverPermissions;

        	return *this;
        }
        
        bool Permission::ServerHasPermission(LocationIDType server) const
        {
        	if (!(server < ALLSTAR_NUM_SERVERS))
        	{
        		return false;
        	}

        	//Check if bit at 'server' position in the bitset is set
        	if (serverPermissions.test(server))
        	{
        		return true;
        	}
        	else
        	{
        		return false;
        	}
        }
        
        void Permission::SetServerPermission(LocationIDType server, bool permission)
        {
        	if (!(server < ALLSTAR_NUM_SERVERS))
			{
				return;
			}

        	//Sets the bit at 'server' position in the bitset to given value.
        	serverPermissions.set(server,permission);
        }

        bool Permission::operator ==(const Permission & check) const
		{
        	return serverPermissions == check.serverPermissions;
		}
    }
}
