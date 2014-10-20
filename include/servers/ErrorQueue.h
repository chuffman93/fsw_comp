/*
 * ErrorQueue.h
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#ifndef ERRORQUEUE_H_
#define ERRORQUEUE_H_

#include "core/Packet.h"
#include "core/Singleton.h"
#include "core/Factory.h"


namespace Phoenix
{
	namespace Servers
	{
		/* Queue used to pass errors from the error handlers to the
		 * Error Octopus in order to process the errors.
		 */

		/*! \brief Length of the Error Queue */
		#define ERROR_QUEUE_LENGTH 100

		class ErrorQueue : public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class 
			*	
			*	This allows factory to call ErrorQueue's private constructor
			*/
			friend class Phoenix::Core::Factory;
		public:
			/*! \brief Enqueues error packet to the error queue.
			 *
			 *  \note error queue must be initialized before this
			 *      function can be called.
			 */

			 bool EnqueueError(Phoenix::Core::Packet * packet);

			/*! \brief Returns the number of packets in the error queue.
			 *
			 *  \note error queue must be initialized before this
			 *      function can be called.
			 */

			 size_t ErrorsWaiting(void);

			/*! \brief Returns the next packet in the error queue.
			 *
			 *  \note error queue must be initialized before this
			 *      function can be called.
			 */
			 Phoenix::Core::Packet * GetNextError(void);

		private:
			 /*! \brief Initialize the ErrorQueue Class
			*
			*  Initializes the operating system constructs needed for
			*  ErrorQueue to work properly.
			*/
			static void Initialize(void);

			/*! \brief Static Destructor for the ErrorQueue
			 *
			 *  Frees all internal memory use and frees all operating system
			 *  objects used.
			 */
#ifdef HOST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if ErrorQueue Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			ErrorQueue(void);
			~ErrorQueue(void);
			ErrorQueue & operator=(const ErrorQueue & source);
			
			mqd_t errQueueHandle;
			struct mq_attr errQueueAttr;
			char * errQueueName;
			int errQinit;
		};
	}
}



#endif /* ERRORQUEUE_H_ */
