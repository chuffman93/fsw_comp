
#include "core/MultiDataMessage.h"
#include <stdio.h>

using namespace std;

namespace Phoenix
{
    namespace Core
    {

        MultiDataMessage::MultiDataMessage()
			:SendMessage(), parameters()
        {
            // Intentionally left empty
        }

        MultiDataMessage::MultiDataMessage(MessageCodeType opcodeIn)
			:SendMessage(opcodeIn), parameters()
        {
            // Intentionally left empty
        }

        MultiDataMessage::MultiDataMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn)
			:SendMessage(opcodeIn)
        {
            AddParameter(parameterIn);
        }

        MultiDataMessage::MultiDataMessage(MessageCodeType opcodeIn, const list<VariableTypeData*> & parametersIn)
			:SendMessage(opcodeIn), parameters()
        {
            SetParameters(parametersIn);
        }

        MultiDataMessage::MultiDataMessage(const MultiDataMessage & source)
			:SendMessage(source)
        {
            SetParameters(source.GetParameters());
        }

        MultiDataMessage::~MultiDataMessage(void )
        {
            // Memory management of the parameters list
            ClearParameters();
        }

        const list<VariableTypeData*> & MultiDataMessage::GetParameters(void) const
        {
            return parameters;
        }

        bool MultiDataMessage::AddParameter (const VariableTypeData & newParam)
        {

            // Add parameter to end of parameters list
            try
            {
                parameters.push_back(new VariableTypeData(newParam));
            }
            // If bad allocation exception caught, return false
            catch (bad_alloc & e)
            {
                return false;
            }

            return true;
        }

        bool MultiDataMessage::SetParameters(const list<VariableTypeData*> & newParams)
        {
            // Memory management of the parameters list
            ClearParameters();

            IteratorType it;

            // Loop through newParams list
            for (it = newParams.begin(); it != newParams.end(); ++it)
            {
                // Add current parameter in newParams, if unsuccessful return false
                if (!(AddParameter(*(*it))))
                {
                    return false;
                }
            }

            return true;
        }

        bool MultiDataMessage::SetParameters(uint8 * buffer, size_t size)
        {
            // Make sure buffer is not false and size is large enough
            if (NULL == buffer || size < 3)
            {
                return false;
            }

            printf("multidatamessage: Setting params\n");
            // Get the "length" field from the buffer (two bytes)
            uint16 length = *buffer++;
            length = length << 8;
            length |= *buffer++;

            // Update the size
            size -= sizeof(uint16);

            // Get the number of parameters from the buffer and update the size
            size_t numParameters = *buffer++;
            size--;

            // Variables to hold current parameter from buffer
            VariableTypeData * parameter;
            size_t parameterSize;

            // Variable to keep track of current length of message
            size_t messageLength = 0;

            printf("Length: %zu, Num parameters: %zu\n", length, numParameters);

            // Loop through the number of parameters in the buffer
            for (size_t i = 0; i < numParameters; i++)
            {
            	printf("MultiDataMes: in for\n");
                // Create new parameter from buffer
                parameter = new VariableTypeData(buffer, size);

                // Get flatten size of current buffer
                parameterSize = parameter->GetFlattenSize();

                // Update current message length, buffer, and size
                messageLength += parameterSize;
                buffer += parameterSize;
                size -= parameterSize;

                // Add the current parameter to the parameters list, if
                // unsuccessful  return false
                if (!AddParameter(*parameter))
                {
                	printf("Failed to add param!\n");
                    return false;
                }

                // Delete the current parameter (it will be copied when added)
                delete parameter;
            }

            printf("messageLength %u actualLength %u\n", messageLength, length);
            // If the actual message length is not what the buffer said it
            // would be, return false

            if (messageLength == length)
            {
            	printf("MultiDataMessage: Created message successfully\n");
                return true;
            }
            else
            {
            	printf("MultiDataMes: returning false\n");
                return false;
            }
        }

        void MultiDataMessage::ClearParameters(void )
        {
            IteratorType it;

            // Loop through the parameters in the parameters list
            for (it = parameters.begin(); it != parameters.end(); ++it)
            {
                // Delete the current parameter
                delete *it;
            }

            // Clear the parameters list
            parameters.clear();
        }

        bool MultiDataMessage::ParametersEqual(const MultiDataMessage & check) const
        {
            IteratorType it1, it2;

            // Make sure both parameters lists are the same size
            if (parameters.size() != check.parameters.size())
            {
                return false;
            }

            // Loop through both parameters lists concurrently
            for (it1 = check.parameters.begin(), it2 = parameters.begin();
                    it1 != check.parameters.end() && it2 != parameters.end();
                    ++it1, ++it2)
            {
                // If the current parameter from each lists are not equal, return false
                if (!(*(*it1) == *(*it2)))
                {
                    return false;
                }
            }

            return true;
        }

        bool MultiDataMessage::operator ==(Message * check) const
		        {
            // Attempt to cast check into a MultiDataMessage
            MultiDataMessage* msg = dynamic_cast<MultiDataMessage*>(check);

            // If unsuccessful, return false.
            if (NULL == msg)
            {
                return false;
            }
            else
            {
                // Check the parent class == operator, and if
                // the parameter lists are the same
                return ((SendMessage::operator==(check)) &&
                        (this->ParametersEqual(*msg)));
            }
		        }

        MultiDataMessage& MultiDataMessage::operator=(const MultiDataMessage & source)
        {
            // Make sure this isn't the same as source
            if (this == &source)
            {
                return *this;
            }

            // Memory management of the parameters list
            ClearParameters();

            SendMessage::operator=(source);

            SetParameters(source.parameters);

            return *this;
        }

        size_t MultiDataMessage::GetFlattenSize(void ) const
        {
            // Flatten size of parent class
            size_t size = SendMessage::GetFlattenSize();

            // Size of "length" field
            size += sizeof(uint16);

            // Size of number of parameters field
            size += sizeof(uint8);

            IteratorType it;

            // Loop through all the parameters in parameters list
            for (it = parameters.begin(); it != parameters.end(); ++it)
            {
                // Size of current parameter in list
                size += (*it)->GetFlattenSize();
            }

            return size;
        }

        size_t MultiDataMessage::Flatten(uint8 * buffer, size_t size) const
        {
            size_t numCopied = 0;

            // Check the buffer and buffer size.
            if (NULL == buffer || size < GetFlattenSize())
            {
                return 0;
            }

            // Flatten the parent class
            numCopied = SendMessage::Flatten(buffer, size);

            // Update the buffer and size
            buffer += numCopied;
            size -= numCopied;

            // Initialize length field to a byte long (size of number of parameters)
            uint16 length = sizeof(uint8);

            IteratorType it;

            // Loop through the parameters
            for (it = parameters.begin(); it != parameters.end(); ++it)
            {
                // Increase length field by size of current parameter
                length += (*it)->GetFlattenSize();
            }

            // Put length field into the buffer
            *buffer++ = (length >> 8) & 0xFF;
            *buffer++ = length & 0xFF;

            // Update numCopied and size
            numCopied += sizeof(uint16);
            size -= sizeof(uint16);

            // Get number of parameters
            uint8 numParameters = parameters.size();

            // Put number of parameters into the buffer
            *buffer++ = numParameters;

            // Update size and numCopied
            ++numCopied;
            --size;

            // Variable to hold size of current parameter
            size_t paramSize;

            // Loop through parameters in parameters list
            for (it = parameters.begin(); it != parameters.end(); ++it)
            {
                // Flatten current parameter into buffer
                paramSize = (*it)->Flatten(buffer, size);

                // Update buffer, numCopied, and size
                buffer += paramSize;
                numCopied += paramSize;
                size -= paramSize;
            }

            return numCopied;
        }
    }
}
