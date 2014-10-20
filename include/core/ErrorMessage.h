/*! \file ErrorMessage.h
 *  \brief Header File for the ErrorMessage Class.
 *
 *  This class defines an ErrorMessage, a MultiDataMessage meant to communicate
 *  an error.
 *
 */

#ifndef _ERRORMESSAGE_H
#define _ERRORMESSAGE_H

#include "core/MultiDataMessage.h"

namespace Phoenix
{
    namespace Core
    {
		enum ErrorOpcodeEnum
		{
			ERROR_OPCODE_SERVER_NOT_REGISTERED = 0,
			ERROR_OPCODE_UNRECOGNIZED_TYPE,
			ERROR_OPCODE_HANDLER_TIMED_OUT,
			ERROR_OPCODE_PACKET_NOT_ALLOWED,
			ERROR_OPCODE_MAX
		};

		/*! \brief ErrorMessage Class
		 *
		 *  This class defines an ErrorMessage, a MultiDataMessage meant to communicate
		 *  an error.
		 *
		 */
        class ErrorMessage : public MultiDataMessage
        {
        public:
        	/*! \brief Default constructor for ErrorMessage.
			 *
			 *  \note Sets the opcode to 0, and uses default VariableTypeData constructor.
			 *
			 */
        	ErrorMessage();

        	/*! \brief Constructor for ErrorMessage with MessageCodeType parameter.
			 *
			 *  \note Uses default VariableTypeData constructor.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *
			 */
            ErrorMessage(MessageCodeType opcodeIn);

            ErrorMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn);

            /*! \brief Constructor for ErrorMessage with MessageCodeType and VariableTypeData parameter.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *  \param dataIn Data of the message.
			 *
			 */
            ErrorMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData*> & parametersIn);

            /*! \brief Copy Constructor for ErrorMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 */
            ErrorMessage(const ErrorMessage & source);

            /*! \brief Destructor for ErrorMessage. */
            virtual ~ErrorMessage(void );

            /*! \brief Returns the type of this message.
			 *
			 *  \return Type of this message.
			 *
			 */
            virtual MessageTypeEnum GetType() const;

            /*! \brief Assignment Operator for ErrorMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return Reference to the calling object.
			 *
			 */
            ErrorMessage & operator=(const ErrorMessage & source);

            /*! \brief Returns a pointer to a new ErrorMessage object that is a copy of the calling object.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return A pointer to a new ErrorMessage object that is a copy of the calling object.
			 *
			 */
			virtual Message * Duplicate(void ) const;
        };
    }
}

#endif  //_ERRORMESSAGE_H
