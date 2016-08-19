/*! \file Arbitrator.h
 *  \brief Header File for the Arbitrator Class
 *
 *  This file contains the definition of the Arbitrator class, which
 *  acts as a permission-checking object for a server.
 */

#ifndef _ARBITRATOR_H
#define _ARBITRATOR_H

#include "core/Arbitrator.h"
#include "core/Permission.h"
#include "core/ACPPacket.h"
#include "core/StdTypes.h"

#include <map>

namespace AllStar{
namespace Core{

/*! \brief Arbitrator Authenticate Status Enumeration
 *
 *  Enumerates the possible return values from the
 *  WaitForDispatchResponse method.
 */
enum ArbitratorAuthStatusEnum
{
	/*! \brief ACPPacket Has Permission */
	ARBITRATOR_AUTH_STATUS_PERMISSION = 0,
	/*! \brief ACPPacket Does Not Have Permission */
	ARBITRATOR_AUTH_STATUS_NO_PERMISSION,
	/*! \brief ACPPacket Does Not Match Server */
	ARBITRATOR_AUTH_STATUS_WRONG_SERVER,
	/*! \brief Maximum Return Value */
	ARBITRATOR_AUTH_STATUS_MAX
};

/*! \brief Class that Checks ACPPacket Permissions for a Server
 *
 *  This class checks if a packet sent from one AllStar server
 *  to another meets the permissions defined for the destination
 *  server.
 */
class Arbitrator{
public:
	/*! \brief Constructor for Arbitrator
	 *
	 *  \param serverIDIn Server to associate the Arbitrator with.
	 */
	Arbitrator(LocationIDType serverIDIn);

	/*! \brief Copy Constructor for Arbitrator
	 *
	 *  \param source Object to be copied.
	 */
	Arbitrator(const Arbitrator & source);

	/*! \brief Destructor for Arbitrator */
	virtual ~Arbitrator(void );

	/*! \brief Assignment Operator for Arbitrator
	 *
	 *  \param source Object to be copied.
	 *
	 *  \return Reference to the calling object.
	 */
	Arbitrator & operator=(const Arbitrator & source);

	/*! \brief Get the Server ID
	 *
	 *  \return Server associated with the Arbitrator object.
	 */
	LocationIDType GetServerID(void ) const;

	/*! \brief Get the Permission for a Message ID
	 *
	 *  \param id Message identifier to get permission for.
	 *  \param permissionOut Reference to permission object.  Set to
	 *  the permission associated with id.
	 *
	 *  \return true if id has a permission associated with and
	 *  false otherwise.
	 */
	bool GetPermission(const MessageIdentifierType & id, Permission & permissionOut) const;

	/*! \brief Authenticate a ACPPacket
	 *
	 *  Checks if the server sending the given packet has the correct
	 *  permissions to send the packet to the Arbitrator's server.
	 *
	 *  \param packet ACPPacket to be checked.
	 *
	 *  \return \ref ARBITRATOR_AUTH_STATUS_PERMISSION if the sending
	 *  server has permission,<br>
	 *  \ref ARBITRATOR_AUTH_STATUS_NO_PERMISSION if the sending
	 *  server does not have permission or if no permission is
	 *  registered for the given message,<br>
	 *  \ref ARBITRATOR_AUTH_STATUS_WRONG_SERVER if the destination
	 *  server in the packet does not match serverID.
	 */
	ArbitratorAuthStatusEnum Authenticate(const ACPPacket & packet) const;

	/*! \brief Modify the Permission Associated with a Message
	 *  Identifier
	 *
	 *  Modifies the permission associated with id if a permission is
	 *  already associated with id.  If no permission has been set for
	 *  id, then the given permission is added to the Arbitrator for
	 *  the given id.
	 *
	 *  \param id Message identifier to modify permission for.
	 *  \param permission Permission to associate with id.
	 *
	 *  \return true if the modification was successful and false
	 *  otherwise.
	 */
	bool ModifyPermission(const MessageIdentifierType & id, const Permission & permission);

	void ClearLocalPermissions(void );

private:
	/*! Typedef for Pairs in the Permission Map */
	typedef std::pair<MessageIdentifierType, Permission *> PairType;

	/*! \brief Typedef for the Iterator Type of the Permission Map */
	typedef std::map<MessageIdentifierType, Permission *>::iterator IteratorType;

	/*! \brief Typedef for the Constant Iterator Type of the Permission Map */
	typedef std::map<MessageIdentifierType, Permission *>::const_iterator ConstIteratorType;

	/*! \brief Server Associated with the Arbitrator */
	LocationIDType serverID;

	/*! \brief Permission Map */
	std::map<MessageIdentifierType, Permission *> permissionMap;

	/*! \brief Copiy the Permission Map from One Arbitrator to Another
	 *
	 *  Performs a deep copy of the permission map from source into
	 *  the local permission map.
	 *
	 *  \param source Object to copy permission map from.
	 */
	void CopySourceToLocalMap(const Arbitrator & source);
};

}
}

#endif  //_ARBITRATOR_H
