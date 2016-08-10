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

namespace AllStar
{
    namespace Core
    {
		/*! \brief Class to encapsulate server action permissions.
		 *
		 *  This class encapsulates the permissions for each server to perform
		 *  some action.
		 */
        class Permission
        {
        public:
            /*! \brief Constructor for Permission from a Bool.
             *
             *  \param allSet Bool for if all permissions should be set or unset.
             */
            Permission(bool allSet = false);

            /*! \brief Copy Constructor for Permission.
             *
             *  \param source Object to be copied.
             */
            Permission(const Permission & source);

            /*! \brief Destructor for Permission. */
            virtual ~Permission(void );

            /*! \brief Assignment Operator for Permission.
             *
             *  \param source Object to be copied.
             *
             *  \return Reference to the calling object.
             */
            Permission & operator=(const Permission & source);

            /*! \brief Returns if the given server has permission.
             *
             *  \param server LocationIDType of the server to check.
             *
             *  \return Whether given server has permission.
             */
            bool ServerHasPermission(LocationIDType server) const;

            /*! \brief Sets the given server to the given permission.
             *
             *  \param server LocationIDType of the server to set.
             *  \param permission Bool to set the permission to.
             *
             *  \return Nothing.
             */
            void SetServerPermission(LocationIDType server, bool permission = true);

            /*! \brief Equals Operator for Permission.
             *
             *  \param check Object to be compared against.
             *  \return true if this object is equivalent to check.
             */
            bool operator ==(const Permission & check) const;
        private:
            /*! \brief Bitset to hold the permissions for each server. */
            std::bitset<ALLSTAR_NUM_SERVERS> serverPermissions;
        };
    }
}

#endif  //_PERMISSION_H
