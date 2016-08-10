/*
 * ACSHandlers.h
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 */

#ifndef ACSHANDLERS_H_
#define ACSHANDLERS_H_

#include "core/MessageHandler.h"
#include "core/ReturnMessage.h"
#include "core/FSWPacket.h"
#include "core/VariableTypeEnum.h"

/* Message handlers for the ACS server.
 * Anytime a message is sent to ACS it should
 * execute one of these handlers.
 */
class ACSMeasurementHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for health and status.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSStateHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for the ACS state.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSRawADCHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for adc values.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSAttitudeErrorHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for current attitude error.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};


class ACSPointGNDHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a command to point to ground coordinates.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 4;
		static uint32 enumArray[numParams];
};


class ACSPointMRPHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a command to point to MRP vector.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 3;
		static uint32 enumArray[numParams];
};

class ACSRequestGNDHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSRequestMRPHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSSunsoakHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSPointNadirHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
		
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

// class ACSSetTargetHandler : public AllStar::Core::MessageHandler
// {
// public:
//         /*! \brief Handle a request for power data.
//          *
//          *  \param packet FSWPacket containing the message.
//          *  \return ReturnMessage that serves as a response.
//          */
//         AllStar::Core::ReturnMessage * Handle(const AllStar::Core::FSWPacket & packet);
// private:
// 		const static uint32 numParams = 4;
// 		static uint32 enumArray[numParams];
// };
// 
// class ACSRangeHandler : public AllStar::Core::MessageHandler
// {
// public:
//         /*! \brief Handle a request for power data.
//          *
//          *  \param packet FSWPacket containing the message.
//          *  \return ReturnMessage that serves as a response.
//          */
//         AllStar::Core::ReturnMessage * Handle(const AllStar::Core::FSWPacket & packet);
// private:
// 		const static uint32 numParams = 1;
// 		static uint32 enumArray[numParams];
// };

class ACSHoldHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSDisableHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSResetHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSGPSHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 8;
		static uint32 enumArray[numParams];
};

class ACSStarCameraHSHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSPointComGndHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class ACSHelloWorldTestHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);

};

// class ACSFunctionalTestHandler : public AllStar::Core::MessageHandler
// {
// public:
//         /*! \brief Handle a request for power data.
//          *
//          *  \param packet FSWPacket containing the message.
//          *  \return ReturnMessage that serves as a response.
//          */
//         AllStar::Core::ReturnMessage * Handle(const AllStar::Core::FSWPacket & packet);
// private:
// 		const static uint32 numParams = 1;
// 		static uint32 enumArray[numParams];
// };
// 
// class ACSFunctionalTestCompleteHandler : public AllStar::Core::MessageHandler
// {
// public:
//         /*! \brief Handle a request for power data.
//          *
//          *  \param packet FSWPacket containing the message.
//          *  \return ReturnMessage that serves as a response.
//          */
//         AllStar::Core::ReturnMessage * Handle(const AllStar::Core::FSWPacket & packet);
// };

class ACSControlOrientationsHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 10;
		static uint32 enumArray[numParams];
};

class ACSMOIWheelsHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 3;
		static uint32 enumArray[numParams];
};

// class ACSMOISatelliteHandler : public AllStar::Core::MessageHandler
// {
// public:
//         /*! \brief Handle a request for power data.
//          *
//          *  \param packet FSWPacket containing the message.
//          *  \return ReturnMessage that serves as a response.
//          */
//         AllStar::Core::ReturnMessage * Handle(const AllStar::Core::FSWPacket & packet);
// private:
// 		const static uint32 numParams = 6;
// 		static uint32 enumArray[numParams];
// };

class ACSTorqCoeffHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 8;
		static uint32 enumArray[numParams];
};

class ACSOrbitEphemHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 5;
		static uint32 enumArray[numParams];
};

class ACSControllerGainsHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 11;
		static uint32 enumArray[numParams];
};

class ACSContolFreqHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 9;
		static uint32 enumArray[numParams];
};

class ACSStarCameraSettingsHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 7;
		static uint32 enumArray[numParams];
};

class ACSRotationModeHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 4;
		static uint32 enumArray[numParams];
};

class ACSUpdateNotchFilterHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 4;
		static uint32 enumArray[numParams];
};

class ACSUpdateLeapSecondHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class ACSGyroZRVHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 4;
		static uint32 enumArray[numParams];
};

class ACSDeguassProfileHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 11;
		static uint32 enumArray[numParams];
};

class ACSUpdateGNDCoordsHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 3;
		static uint32 enumArray[numParams];
};

class ACSUseGyroBiasHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a request for power data.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};


class ACSErrorHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle an error.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

#endif /* ACSHANDLERS_H_ */
