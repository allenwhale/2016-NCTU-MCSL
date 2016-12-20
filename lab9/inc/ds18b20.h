/*
 * ds18b20.h
 *
 *  Created on: 2016¦~12¤ë18¤é
 *      Author: Administrator
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "gpio.h"
#include "delay.h"
#include "stm32l476xx.h"

#define DS18B20_SKIP_ROM 0xCC
#define DS18B20_SEARCH_ROM 0xF0
#define DS18B20_READ_ROM 0x33
#define DS18B20_MATCH_ROM 0x55
#define DS18B20_ALARM_ROM 0xEC
#define DS18B20_START_CONVERT 0x44
#define DS18B20_READ_SCRATCHPAD 0xBE
#define DS18B20_WRITE_SCRATCHPAD 0x4E
#define DS18B20_COPY_SCRATCHPAD 0x48
#define DS18B20_RECALL_EEPROM 0xB8
#define DS18B20_READ_POWER 0xB4
#define DS18B20_ALARM_TH 0x64
#define DS18B20_ALARM_TL 0x8a
#define DS18B20_PRECISION_9BIT 0x1f
#define DS18B20_PRECISION_10BIT 0x3f
#define DS18B20_PRECISION_11BIT 0x5f
#define DS18B20_PRECISION_12BIT 0x7f
#define ENABLE_SYSTICK() { \
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; \
}
#define DISABLE_SYSTICK() { \
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; \
}

void DS18B20Reset(GPIOPin *dq);
void DS18B20WriteBit(GPIOPin *dq, int value);
void DS18B20WriteByte(GPIOPin *dq, int value);
int DS18B20ReadBit(GPIOPin *dq);
int DS18B20ReadByte(GPIOPin *dq);
void DS18B20StartConvert(GPIOPin *dq);
double DS18B20Read(GPIOPin *dq);
void DS18B20Init(GPIOPin *dq, int precision);


#endif /* DS18B20_H_ */
