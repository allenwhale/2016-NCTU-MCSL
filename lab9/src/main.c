/*
 * main.c
 *
 *  Created on: 2016¦~12¤ë16¤é
 *      Author: Administrator
 */
#include "stm32l476xx.h"
#include "lcd.h"
#include "gpio.h"
#include "delay.h"
#include "ds18b20.h"
#include <stdio.h>
LCD1602A lcd;
GPIOPin dq;
const char *teamNo = "10";
int nowPos = 0, status = 0;
const char *nowChar;
const char *printString = "test test!";
const char *customString = "\x01\x02";
const unsigned charMap[2][8] = {
		{1, 2, 3, 4, 5, 6, 7, 8},
		{8, 7, 6, 5, 4, 3, 2, 1}
};
void SysClock(){
	int plln = 20;
	int pllm = 1;
	int pllr = 1;
	plln <<= RCC_PLLCFGR_PLLN_Pos;
	pllm <<= RCC_PLLCFGR_PLLM_Pos;
	pllr <<= RCC_PLLCFGR_PLLR_Pos;
	RCC->CFGR &= ~(RCC_CFGR_SW_Msk);
	RCC->CR &= ~(RCC_CR_PLLON);
	while((RCC->CR & RCC_CR_PLLRDY));
	//config
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN_Msk);
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk);
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLR_Msk);
	RCC->PLLCFGR |= plln;
	RCC->PLLCFGR |= pllm;
	RCC->PLLCFGR |= pllr;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;
	RCC->CR |= RCC_CR_PLLON;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}
void LCDInit(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	for(int i=0;i<=10;i++){
		GPIOB->MODER &= ~(3 << (i * 2));
		GPIOB->MODER |= (1 << (i * 2));
	}
	GPIOPin gpio;
	for(int i=0;i<8;i++){
		GPIOInit(&gpio, GPIOB, i);
		LCDSetDATA(&lcd, &gpio, i);
	}
	GPIOInit(&gpio, GPIOB, 8);
	LCDSetRS(&lcd, &gpio);
	GPIOInit(&gpio, GPIOB, 9);
	LCDSetRW(&lcd, &gpio);
	GPIOInit(&gpio, GPIOB, 10);
	LCDSetEN(&lcd, &gpio);
}
void SysTickConfig(int tick){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	SysTick->LOAD = tick & SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
void SysTickHandlerLab91(){
	static int nowPos = 0;
	LCDClearScreen(&lcd);
	LCDPrintString(&lcd, teamNo, nowPos >> 4, nowPos & 0xF);
	nowPos = (nowPos + 1) & 31;
}
void SysTickHandlerLab92(){
	if(status == 0){
		LCDClearScreen(&lcd);
		LCDPrintString(&lcd, customString, nowPos >> 4, nowPos & 0xF);
		nowPos = (nowPos + 1) & 31;
	}else if(status == 1){
		if(*nowChar == 0){
			nowChar = printString;
			LCDGotoXY(&lcd, 0, 0);
			LCDClearScreen(&lcd);
		}
		LCDSendData(&lcd, *nowChar);
		nowChar++;
	}
}
void SysTickHandlerLab93(){
	if(status == 0){
		LCDClearScreen(&lcd);
		LCDPrintString(&lcd, customString, nowPos >> 4, nowPos & 0xF);
		nowPos = (nowPos + 1) & 31;
	}else if(status == 1){
		DS18B20StartConvert(&dq);
		double temp = DS18B20Read(&dq);
		char s[10];
		sprintf(s, "%.4f", temp);
		LCDPrintString(&lcd, s, 0, 0);
	}
}
void SysTick_Handler(){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		SysTickHandlerLab93();
	}
}
void lab91(){
	SysTickConfig(10000000 / 10 * 3);
}
int read_button(){
	int cnt = 0;
	for(int i=200;i;i--){
		cnt += (GPIOC->IDR >> 13) & 1;
	}
	return cnt < 100;
}
void lab92(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(3 << (13 * 2));
	SysTickConfig(10000000 / 10 * 3);
	nowPos = 0;
	status = 0;
	nowChar = printString;
	int lastButton = 0;
	while(1){
		int nowButton = read_button();
		if(!lastButton && nowButton){
			status ^= 1;
			if(status == 0){
				nowPos = 0;
				LCDClearScreen(&lcd);
			}else if(status == 1){
				nowChar = printString;
				LCDClearScreen(&lcd);
			}
		}
		lastButton = nowButton;
	}
}
void lab93(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(3 << (0 * 2));
	GPIOA->MODER |= (1 << 0);
	GPIOA->OTYPER |= (1 << 0);
	GPIOInit(&dq, GPIOA, 0);
	DS18B20Init(&dq, DS18B20_PRECISION_12BIT);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(3 << (13 * 2));
	SysTickConfig(10000000 / 10 * 3);
	nowPos = 0;
	status = 0;
	nowChar = printString;
	int lastButton = 0;
	while(1){
		int nowButton = read_button();
		if(!lastButton && nowButton){
			status ^= 1;
			if(status == 0){
				nowPos = 0;
				LCDClearScreen(&lcd);
				SysTickConfig(10000000 / 10 * 3);
			}else if(status == 1){
				LCDClearScreen(&lcd);
				SysTickConfig(10000000);
			}
		}
		lastButton = nowButton;
	}
}
int main(){
	SCB->CPACR |= (0xF << 20);
	__DSB();
	__ISB();
	SysClock();
	LCDInit();
	LCDInitSetting(&lcd);
	LCDCreateChar(&lcd, 1, charMap[0]);
	LCDCreateChar(&lcd, 2, charMap[1]);
	LCDGotoXY(&lcd, 0, 0);
	LCDSendData(&lcd, 1);
	LCDSendData(&lcd, 2);
	//lab91();
	//lab92();
	lab93();

	while(1);
}

