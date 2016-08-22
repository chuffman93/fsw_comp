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

class Arbitrator{
public:
	Arbitrator(LocationIDType serverIDIn);
	Arbitrator(const Arbitrator & source);

	virtual ~Arbitrator(void );

	Arbitrator & operator=(const Arbitrator & source);

	LocationIDType GetServerID(void ) const;

	bool GetPermission(const MessageIdentifierType & id, Permission & permissionOut) const;

	ArbitratorAuthStatusEnum Authenticate(const ACPPacket & packet) const;

	bool ModifyPermission(const MessageIdentifierType & id, const Permission & permission);

	void ClearLocalPermissions(void );

private:
	typedef std::pair<MessageIdentifierType, Permission *> PairType;
	typedef std::map<MessageIdentifierType, Permission *>::iterator IteratorType;
	typedef std::map<MessageIdentifierType, Permission *>::const_iterator ConstIteratorType;

	LocationIDType serverID;

	std::map<MessageIdentifierType, Permission *> permissionMap;

	void CopySourceToLocalMap(const Arbitrator & source);
};

}
}

#endif  //_ARBITRATOR_H
