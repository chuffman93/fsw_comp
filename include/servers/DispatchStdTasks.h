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
