/*! \file VariableTypeEnum.h
 *  \brief Header File for the VariableTypeEnum Enumeration.
 *
 *  This enumeration is a partial type enumeration of the types listed in
 *  RF 1832.
 */

#ifndef _VARIABLETYPEENUM_H
#define _VARIABLETYPEENUM_H

#define VAR_TYPE_ENUM_VOID_SIZE 		0
#define VAR_TYPE_ENUM_INT_SIZE 			4
#define VAR_TYPE_ENUM_UNSIGNED_INT_SIZE	4
#define VAR_TYPE_ENUM_BOOL_SIZE			1
#define VAR_TYPE_ENUM_FLOAT_SIZE		4
#define VAR_TYPE_ENUM_DOUBLE_SIZE		8
#define VAR_TYPE_ENUM_STRING_SIZE		4
#define VAR_TYPE_ENUM_ARRAY_SIZE		4

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Enumeration of supported types for VariableTypeData */
        enum VariableTypeEnum
        {
            VAR_TYPE_ENUM_VOID = 0,			//!< Void
            VAR_TYPE_ENUM_INT,				//!< Integer
            VAR_TYPE_ENUM_UNSIGNED_INT,		//!< Unsigned Integer
            VAR_TYPE_ENUM_BOOL,				//!< Boolean
            VAR_TYPE_ENUM_FLOAT,			//!< Single-Precision Floating Point
            VAR_TYPE_ENUM_DOUBLE,			//!< Double-Precision Floating Point
            VAR_TYPE_ENUM_STRING,			//!< String
            VAR_TYPE_ENUM_ARRAY,			//!< Variable-Length Array
			VAR_TYPE_ENUM_PACKET,			//!< Variable-Length Array
            VAR_TYPE_ENUM_NUM_TYPES			//!< Number of Types
        };
    }
}

#endif  //_VARIABLETYPEENUM_H
