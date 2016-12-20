/*
 * lcd.c
 *
 *  Created on: 2016¦~12¤ë16¤é
 *      Author: Administrator
 */
#include "lcd.h"


void LCDSetRS(LCD1602A *lcd, GPIOPin *gpio){
	GPIOInit(&lcd->RS, gpio->GPIO, gpio->pin);
}
void LCDSetRW(LCD1602A *lcd, GPIOPin *gpio){
	GPIOInit(&lcd->RW, gpio->GPIO, gpio->pin);
}
void LCDSetEN(LCD1602A *lcd, GPIOPin *gpio){
	GPIOInit(&lcd->EN, gpio->GPIO, gpio->pin);
}
void LCDSetDATA(LCD1602A *lcd, GPIOPin *gpio, int num){
	GPIOInit(&lcd->DATA[num], gpio->GPIO, gpio->pin);
}

void LCDSendRS(LCD1602A *lcd, int value){
	GPIOWriteBit(&lcd->RS, value);
}

void LCDSendRW(LCD1602A *lcd, int value){
	GPIOWriteBit(&lcd->RW, value);
}

void LCDSendEN(LCD1602A *lcd, int value){
	GPIOWriteBit(&lcd->EN, value);
}


void LCDSendDATA(LCD1602A *lcd, int data){
	for(int i=0;i<8;i++){
		GPIOWriteBit(&lcd->DATA[i], data & 1);
		data >>= 1;
	}
}

void LCDPulseEN(LCD1602A *lcd){
	LCDSendEN(lcd, LCD_EN_ENABLE);
	delay_ms(10);
	LCDSendEN(lcd, LCD_EN_DISABLE);
}

void LCDSendData(LCD1602A *lcd, int data){
	LCDSendRS(lcd, LCD_RS_DATA);
	delay_us(100);
	LCDSendRW(lcd, LCD_RW_WRITE);
	delay_us(100);
	LCDSendDATA(lcd, data);
	delay_us(500);
	LCDPulseEN(lcd);
	delay_us(100);
}

void LCDSendCommand(LCD1602A *lcd, int data){
	LCDSendRS(lcd, LCD_RS_COMMAND);
	delay_us(100);
	LCDSendRW(lcd, LCD_RW_WRITE);
	delay_us(100);
	LCDSendDATA(lcd, data);
	delay_us(500);
	LCDPulseEN(lcd);
	delay_us(100);
}

void LCDInitSetting(LCD1602A *lcd){
	delay_ms(30);
	LCDSendCommand(lcd, 0x38);
	delay_ms(5);
	LCDSendCommand(lcd, 0x6);
	delay_ms(5);
	LCDDisplaySetting(lcd, 1, 0, 0);
	LCDClearScreen(lcd);
}

void LCDDisplaySetting(LCD1602A *lcd, int D, int C, int B){
	int data = 0b00001000;
	data |= B << 0;
	data |= C << 1;
	data |= D << 2;
	LCDSendCommand(lcd, data);
	delay_ms(1);
}


void LCDGotoXY(LCD1602A *lcd, int x, int y){
	LCDSendCommand(lcd, (x * 0x40 + y) | 0x80);
	delay_ms(1);
}

void LCDClearScreen(LCD1602A *lcd){
	LCDSendCommand(lcd, 0x01);
	delay_ms(5);
}

void LCDPrintString(LCD1602A *lcd, const char* s, int x, int y){
	LCDGotoXY(lcd, x, y);
	while(*s){
		LCDSendData(lcd, *(s++));
		if(++y == 16){
			x = (x + 1) & 1, y = 0;
			LCDGotoXY(lcd, x, y);
		}
	}
}

void LCDCreateChar(LCD1602A *lcd, int pos, const unsigned *charMap){
	LCDSendCommand(lcd, 0x40 | ((pos & 7) << 3));
	delay_ms(1);
	for(int i=0;i<8;i++){
		LCDSendData(lcd, charMap[i]);
		delay_ms(1);
	}
}
