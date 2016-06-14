/*
 * DispatchStdTasks.h
 *
 * Created: 7/26/2013 11:22:36 AM
 *  Author: Admin
 */ 


#ifndef DISPATCHSTDTASKS_H_
#define DISPATCHSTDTASKS_H_

#include <list>

#include "core/ReturnMessage.h"
#include "core/MultiDataMessage.h"
#include "core/StdTypes.h"
#include "core/VariableTypeEnum.h"

#define DIS_DEBUG			0

#if DIS_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#endif // DEBUG

namespace Phoenix
{
	namespace Servers
	{
		/*!	\brief Dispatches a packet.
		*
		*	\param packet: Fully constructed packet to send.
		*
		*	\return NULL if improperly formated, error message if dispatch fails,
		*	and the response message if dispatched successfully.
		*/
		
		Phoenix::Core::FSWPacket * DispatchPacket(Phoenix::Core::FSWPacket * packet);
		
		/*!	\brief Dispatches a message with no parameters.
		*
		*	\param source: source of the packet to send.
		*	\param destination: destination of the packet to send.
		*	\param number: number field for the packet to send.
		*	\param timestamp: timestamp field for the packet to send.
		*	\param type: type of message to construct for the packet to send.
		*	\param opCode: opCode of the message to construct for the packet to send.
		*	
		*	\return NULL if improperly formated, error message if dispatch fails,
		*	and the response message if dispatched successfully.
		*/
		Phoenix::Core::ReturnMessage * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
				uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode);

		Phoenix::Core::FSWPacket * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
						uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode, int testvar);
				
		/*!	\brief Dispatches a message with one parameter.
		*
		*	\param source: source of the packet to send.
		*	\param destination: destination of the packet to send.
		*	\param number: number field for the packet to send.
		*	\param timestamp: timestamp field for the packet to send.
		*	\param type: type of message to construct for the packet to send.
		*	\param opCode: opCode of the message to construct for the packet to send.
		*	\param parameterIn: single parameter to add to the message.
		*	
		*	\return NULL if improperly formated, error message if dispatch fails,
		*	and the response message if dispatched successfully.
		*/
		Phoenix::Core::ReturnMessage * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
				uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode, const Phoenix::Core::VariableTypeData & parameterIn);
				
		/*!	\brief Dispatches a message with a list parameters.
		*
		*	\param source: source of the packet to send.
		*	\param destination: destination of the packet to send.
		*	\param number: number field for the packet to send.
		*	\param timestamp: timestamp field for the packet to send.
		*	\param type: type of message to construct for the packet to send.
		*	\param opCode: opCode of the message to construct for the packet to send.
		*	\param parametersIn: parameter list to set for the message.
		*
		*	\return NULL if improperly formated, error message if dispatch fails,
		*	and the response message if dispatched successfully.
		*/
		Phoenix::Core::ReturnMessage * DispatchPacket(LocationIDType source, LocationIDType destination, uint16 number,
				uint32 timestamp, MessageTypeEnum type, MessageCodeType opCode, const std::list<Phoenix::Core::VariableTypeData*> & parametersIn);
		
		/*!	\brief Extracts parameters from a packet.
		*
		*	\param packet: packet to extract parameters from.
		*	\param inputParameters: array of VariableTypeEnum to extract.
		*	\param numParams: number of parameters to extract.
		*	\param outputParameters: array to store parameters in.
		*
		*	\return false if type of parameter does not match the inputParameters array 
		*		or if the number of parameters does not match the numParams.
		*		Otherwise returns true if all parameters are extracted correctly and stored
		*		in outputParameters.
		*/
		bool ExtractParameters(const Phoenix::Core::FSWPacket & packet, uint32 * inputParameters,
				uint32 numParams, void ** outputParameters);
				
		/*!	\brief Extracts parameters from a return message.
		*
		*	\param retMsg: return message to extract parameters from.
		*	\param inputParameters: array of VariableTypeEnum to extract.
		*	\param numParams: number of parameters to extract.
		*	\param outputParameters: array to store parameters in.
		*
		*	\return false if type of parameter does not match the inputParameters array 
		*		or if the number of parameters does not match the numParams.
		*		Otherwise returns true if all parameters are extracted correctly and stored
		*		in outputParameters.
		*/
		bool ExtractParameters(const Phoenix::Core::ReturnMessage & retMsg, uint32 * inputParameters,
				uint32 numParams, void ** outputParameters);
				
		/*!	\brief Extracts parameters from a MultiDataMessage.
		*
		*	\param dataMessage: MultiDataMessage to extract parameters from.
		*	\param inputParameters: array of VariableTypeEnum to extract.
		*	\param numParams: number of parameters to extract.
		*	\param outputParameters: array to store parameters in.
		*
		*	\return false if type of parameter does not match the inputParameters array 
		*		or if the number of parameters does not match the numParams.
		*		Otherwise returns true if all parameters are extracted correctly and stored
		*		in outputParameters.
		*/
		bool ExtractParameters(const Phoenix::Core::MultiDataMessage & dataMessage, uint32 * inputParameters,
				uint32 numParams, void ** outputParameters);

		// Functions for extracting parameters into buffers
		uint32 	GetUInt(uint8 * buffer);
		uint16 	GetUInt16(uint8 * buffer);
		int32 	GetInt(uint8 * buffer);
		bool 	GetBool(uint8 * buffer);
		float 	GetFloat(uint8 * buffer);
		double 	GetDouble(uint8 * buffer);

		// Functions for adding parameters to buffers
		void AddUInt32(uint8 * buffer, uint32 data);
		void AddFloat(uint8 * buffer, float data);

		/*!	\brief Handles a message. If message is an error sends it to the error octopus, otherwise writes
		*		it writes it to a file.
		*
		*	\param ServerLocationIDEnum: Server that is processing the message.
		*	\param ReturnMessage: ReturnMessage to process..
		*
		*/	
		void MessageProcess(LocationIDType id, Phoenix::Core::ReturnMessage * retMsg);

		void PacketProcess(LocationIDType id, Phoenix::Core::FSWPacket * retPacket);
	}
}


#endif /* DISPATCHSTDTASKS_H_ */
