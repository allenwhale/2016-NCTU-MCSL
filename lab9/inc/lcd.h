/*
 * lcd.h
 *
 *  Created on: 2016¦~12¤ë16¤é
 *      Author: Administrator
 */

#ifndef LCD_H_
#define LCD_H_
#include "gpio.h"
#include "delay.h"

#define LCD_RS_COMMAND 0
#define LCD_RS_DATA 1
#define LCD_RW_READ 1
#define LCD_RW_WRITE 0
#define LCD_EN_DISABLE 0
#define LCD_EN_ENABLE 1
#define LCD_DISPLAY_ON 1
#define LCD_DISPLAY_OFF 0
#define LCD_CURSOR_ON 1
#define LCD_CURDOR_OFF 0
#define LCD_CURSOR_BLINK_ON 1
#define LCD_CURSOR_BLINK_OFF 0

typedef struct LCD1602A_t{
	GPIOPin VS, RS, RW, EN, DATA[8];
} LCD1602A;

void LCDSetVS(LCD1602A *lcd, GPIOPin *gpio);
void LCDSetRS(LCD1602A *lcd, GPIOPin *gpio);
void LCDSetRW(LCD1602A *lcd, GPIOPin *gpio);
void LCDSetEN(LCD1602A *lcd, GPIOPin *gpio);
void LCDSetDATA(LCD1602A *lcd, GPIOPin *gpio, int num);
void LCDSendRS(LCD1602A *lcd, int value);
void LCDSendRW(LCD1602A *lcd, int value);
void LCDSendEN(LCD1602A *lcd, int value);
void LCDSendDATA(LCD1602A *lcd, int data);
void LCDPulseEN(LCD1602A *lcs);
void LCDSendData(LCD1602A *lcd, int data);
void LCDSendCommand(LCD1602A *lcd, int data);
void LCDInitSetting(LCD1602A *lcd);
void LCDDisplaySetting(LCD1602A *lcd, int D, int C, int B);
void LCDGotoXY(LCD1602A *lcd, int x, int y);
void LCDClearScreen(LCD1602A *lcd);
void LCDPrintString(LCD1602A *lcd, const char* s, int x, int y);
void LCDCreateChar(LCD1602A *lcd, int pos, const unsigned *charMap);

#endif /* LCD_H_ */
