/*
 * usart.h
 *
 *  Created on: 2016¦~12¤ë23¤é
 *      Author: Administrator
 */

#ifndef USART_H_
#define USART_H_
#include "stm32l476xx.h"

void USARTSetBaudRate(USART_TypeDef *usart, int cpuRate, int baudRate);
void USARTSetWordLength(USART_TypeDef *usart, int bits);
void USARTSetStopBit(USART_TypeDef *usart, int bits);
void USARTEnableRXTX(USART_TypeDef *usart, int rx, int tx);
void USARTEnable(USART_TypeDef *usart);
int USARTGetFlagStatus(USART_TypeDef *usart, int flag);
void USARTSendData(USART_TypeDef *usart, int data);
int USARTGetData(USART_TypeDef *usart);
void USARTPrintString(USART_TypeDef *usart, const char *ptr);
int USARTGetLine(USART_TypeDef *usart, char *ptr, int len);

#endif /* USART_H_ */
