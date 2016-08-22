/*! \file Permission.h
 *  \brief Header File for the Permission Class.
 *
 *  This class encapsulates the permissions for each server to perform some action.
 */


#ifndef _PERMISSION_H
#define _PERMISSION_H

#undef min
#include <bitset>

#include "core/Core.h"
#include "core/StdTypes.h"

namespace AllStar{
namespace Core{

class Permission{
public:
	Permission(bool allSet = false);

	Permission(const Permission & source);

	virtual ~Permission(void );

	Permission & operator=(const Permission & source);

	bool ServerHasPermission(LocationIDType server) const;

	void SetServerPermission(LocationIDType server, bool permission = true);

	bool operator ==(const Permission & check) const;

private:
	std::bitset<ALLSTAR_NUM_SERVERS> serverPermissions;
};

}
}

#endif  //_PERMISSION_H
