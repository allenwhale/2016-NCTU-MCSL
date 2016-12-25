/*
 * gpio.h
 *
 *  Created on: 2016¦~12¤ë16¤é
 *      Author: Administrator
 */

#ifndef GPIO_H_
#define GPIO_H_
#include "stm32l476xx.h"

typedef struct GPIOPin_t{
	GPIO_TypeDef *GPIO;
	int pin;
} GPIOPin;

#define GPIOInit(gpio, GPIO_PORT, GPIO_PIN) { \
	(gpio)->GPIO = GPIO_PORT; \
	(gpio)->pin = GPIO_PIN; \
}

#define GPIOSetBit(gpio) { \
	(gpio)->GPIO->BSRR = 1 << (gpio)->pin; \
}

#define GPIOClearBit(gpio) { \
	(gpio)->GPIO->BRR = 1 << (gpio)->pin; \
}

#define GPIOWriteBit(gpio, value) { \
	if((value)){ \
		GPIOSetBit((gpio)); \
	}else{ \
		GPIOClearBit((gpio)); \
	} \
}

#define GPIOReadBit(gpio) (((gpio)->GPIO->IDR >> (gpio)->pin) & 1)

#endif /* GPIO_H_ */
