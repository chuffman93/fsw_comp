/*
 * SCHHandlers.h
 *
 * Created: 11/7/2013 11:34:21 AM
 *  Author: cypa8074
 */ 


#ifndef SCHHANDLERS_H_
#define SCHHANDLERS_H_
#include "core/MessageHandler.h"
#include "servers/SCHStdTasks.h"

class SCHScheduleHandler: public Phoenix::Core::MessageHandler
{
public:
		/* \breif Handle a build schedule request.
		 *
		 *	\param packet FSWPacket containing the command.
		 *	\return ReturnMessage from SCH Hardware.
		 */	
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class SCHPldScheduleHandler: public Phoenix::Core::MessageHandler
{
public:
		/* \breif Handle a build PLD schedule request.
		 *
		 *	\param packet FSWPacket containing the command.
		 *	\return ReturnMessage from SCH Hardware.
		 */	
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
private:		
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class SCHRunScheduleHandler: public Phoenix::Core::MessageHandler
{
public:
		/* \breif Handle a run schedule request.
		 *
		 *	\param packet FSWPacket containing the command.
		 *	\return ReturnMessage from SCH Hardware.
		 */	
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
};

class SCHDefaultRangeHandler: public Phoenix::Core::MessageHandler
{
public:
		/* \breif Handle a new default range to ground station.
		 *
		 *	\param packet FSWPacket containing the command.
		 *	\return ReturnMessage from SCH Hardware.
		 */	
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

#endif /* SCHHANDLERS_H_ */
