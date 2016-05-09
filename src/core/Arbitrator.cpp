/*! \file Arbitrator.cpp
 *  \brief Implementation File for the Arbitrator Class
 *
 *  This file contains the implementation of the Arbitrator class, which
 *  acts as a permission-checking object for a server.
 */

#include "core/Arbitrator.h"
#include <iostream>

using namespace std;

namespace Phoenix
{
    namespace Core
    {
        Arbitrator::Arbitrator(LocationIDType serverIDIn)
                   : serverID(serverIDIn), permissionMap( )
        {
        	// Left Intentionally Empty
        }
        
        Arbitrator::Arbitrator(const Arbitrator & source)
                   : serverID(source.serverID)
        {
        	CopySourceToLocalMap(source);
        }
        
        Arbitrator::~Arbitrator(void )
        {
        	ClearLocalPermissions( );
        }
        
        Arbitrator & Arbitrator::operator=(const Arbitrator & source)
        {
        	// If the passed in object is the same as the calling object, then
        	// just return.
        	if (this == &source)
        	{
        		return *this;
        	}

        	// Clear the previous permissions map.
        	ClearLocalPermissions( );

        	// Copy the server ID and permissions.
        	serverID = source.serverID;
        	CopySourceToLocalMap(source);

        	return *this;
        }
        
        LocationIDType Arbitrator::GetServerID(void ) const
        {
            return serverID;
        }

        bool Arbitrator::GetPermission(const MessageIdentifierType & id, Permission & permissionOut) const
        {
        	// Find the permission, if it exists
        	ConstIteratorType it = permissionMap.find(id);

        	if (it == permissionMap.end())
        	{
        		// The permission doesn't exist, so return false.
        		return false;
        	}
        	else
        	{
        		// The permission exists, so set the output variable and
        		// return true.
        		permissionOut = *(it->second);
        		return true;
        	}
        }
        
        ArbitratorAuthStatusEnum Arbitrator::Authenticate(const FSWPacket & packet) const
        {
        	Message * msg;
        	ConstIteratorType it;

        	// Check that the packet matches the Arbitrator's server.
        	if (packet.GetDestination() != serverID)
        	{
        		return ARBITRATOR_AUTH_STATUS_WRONG_SERVER;
        	}

        	// Find the permission for the given packet.
        	msg = packet.GetMessagePtr();
			
        	it = permissionMap.find(MessageIdentifierType(msg->GetType(), msg->GetOpcode()));

        	if (it == permissionMap.end())
			{
        		// The permission doesn't exist, so return false.
				return ARBITRATOR_AUTH_STATUS_NO_PERMISSION;
			}
			else
			{
				// The permission exists, so check if the packet is allowed.
				return ((it->second->ServerHasPermission(packet.GetSource())) ?
						ARBITRATOR_AUTH_STATUS_PERMISSION :
						ARBITRATOR_AUTH_STATUS_NO_PERMISSION);
			}
        }
        
        bool Arbitrator::ModifyPermission(const MessageIdentifierType & id, const Permission & permission)
        {
        	// Find the permission, if it exists.
        	IteratorType it = permissionMap.find(id);

			if (it == permissionMap.end())
			{
				// The permission does not exist, so add it to the map.
				try
				{
					Permission * p = new Permission(permission);
					pair<IteratorType, bool> ret = permissionMap.insert(PairType(id, p));
					return ret.second;
				}
				catch (bad_alloc e)
				{
					return false;
				}
			}
			else
			{
				// The permission exists, so first delete the old one and then
				// add the new permission to the map.
				delete it->second;
				try
				{
					it->second = new Permission(permission);
					return true;
				}
				catch (bad_alloc e)
				{
					return false;
				}
			}
        }
        
        void Arbitrator::ClearLocalPermissions(void )
        {
        	// Iterate through the map and delete each of the permission
        	// objects pointed to by each entry in the map.
        	for (IteratorType it = permissionMap.begin(); it != permissionMap.end(); ++it)
			{
				delete it->second;
			}

        	// Clear the permission map.
        	permissionMap.clear();
        }

        void Arbitrator::CopySourceToLocalMap(const Arbitrator & source)
        {
        	// Iterate through the map of source and copy each permission to
        	// the local map.
        	for (ConstIteratorType it = source.permissionMap.begin(); it != source.permissionMap.end(); ++it)
			{
				ModifyPermission(it->first, *(it->second));
			}
        }
    }
}
