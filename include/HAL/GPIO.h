/*
 * GPIO.h
 *
 * Created: 8/12/2013 8:28:19 PM
 *  Author: Admin
 */ 


#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

#ifdef __cplusplus
namespace AllStar
{
	namespace HAL
	{
		typedef enum 
		{
			PIN_TYPE_NONE = 0,
			PIN_TYPE_GPIO,
			PIN_TYPE_EXPANDER
		} PinType;

		bool SetPin(unsigned int pin, PinType type);
		bool ClearPin(unsigned int pin, PinType type);
		bool GetPin(unsigned int pin, AllStar::HAL::PinType type, unsigned char *value);
	}
}
#endif //__cplusplus


#endif /* _HAL_GPIO_H_ */