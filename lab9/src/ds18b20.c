/*
 * ds18b20.c
 *
 *  Created on: 2016¦~12¤ë18¤é
 *      Author: Administrator
 */
#include "ds18b20.h"

void DS18B20Reset(GPIOPin *dq){
	DISABLE_SYSTICK();
	GPIOSetBit(dq);
	delay_us(50);
	GPIOClearBit(dq);
	delay_us(500);
	GPIOSetBit(dq);
	delay_us(40);
	while(GPIOReadBit(dq));
	delay_us(500);
	GPIOSetBit(dq);
	ENABLE_SYSTICK();
}
void DS18B20WriteBit(GPIOPin *dq, int value){
	DISABLE_SYSTICK();
	GPIOClearBit(dq);
	delay_us(5);
	if(value){
		GPIOSetBit(dq);
	}else{
		GPIOClearBit(dq);
	}
	delay_us(65);
	GPIOSetBit(dq);
	delay_us(2);
	ENABLE_SYSTICK();
}
void DS18B20WriteByte(GPIOPin *dq, int value){
	DISABLE_SYSTICK();
	for(int i=8;i--;){
		DS18B20WriteBit(dq, value & 1);
		value >>= 1;
	}
	ENABLE_SYSTICK();
}
int DS18B20ReadBit(GPIOPin *dq){
	DISABLE_SYSTICK();
	int res;
	GPIOClearBit(dq);
	delay_us(4);
	GPIOSetBit(dq);
	delay_us(4);
	if(GPIOReadBit(dq)){
		res = 1;
	}else{
		res = 0;
	}
	delay_us(65);
	GPIOSetBit(dq);
	ENABLE_SYSTICK();
	return res;
}
int DS18B20ReadByte(GPIOPin *dq){
	DISABLE_SYSTICK();
	int res = 0;
	for(int i=8;i--;){
		res >>= 1;
		if(DS18B20ReadBit(dq)){
			res |= 0x80;
		}else{
			res &= 0x7f;
		}
	}
	ENABLE_SYSTICK();
	return res;
}
void DS18B20StartConvert(GPIOPin *dq){
	DISABLE_SYSTICK();
	DS18B20Reset(dq);
	DS18B20WriteByte(dq, DS18B20_SKIP_ROM);
	DS18B20WriteByte(dq, DS18B20_START_CONVERT);
	while(!GPIOReadBit(dq));
	ENABLE_SYSTICK();
}
double DS18B20Read(GPIOPin *dq){
	DISABLE_SYSTICK();
	DS18B20Reset(dq);
	DS18B20WriteByte(dq, DS18B20_SKIP_ROM);
	DS18B20WriteByte(dq, DS18B20_READ_SCRATCHPAD);
	int low = DS18B20ReadByte(dq);
	int high = DS18B20ReadByte(dq);
	DS18B20Reset(dq);
	DS18B20StartConvert(dq);
	double res = ((high & 0b111) << 8) | low;
	res /= 16;
	ENABLE_SYSTICK();
	return res;
}
void DS18B20Init(GPIOPin *dq, int precision){
	DISABLE_SYSTICK();
	DS18B20Reset(dq);
	DS18B20WriteByte(dq, DS18B20_SKIP_ROM);
	DS18B20WriteByte(dq, DS18B20_WRITE_SCRATCHPAD);
	DS18B20WriteByte(dq, DS18B20_ALARM_TH);
	DS18B20WriteByte(dq, DS18B20_ALARM_TL);
	DS18B20WriteByte(dq, precision);
	DS18B20Reset(dq);
	DS18B20WriteByte(dq, DS18B20_SKIP_ROM);
	DS18B20WriteByte(dq, DS18B20_COPY_SCRATCHPAD);
	while(!GPIOReadBit(dq));
	ENABLE_SYSTICK();
}

