/*
 * usart.c
 *
 *  Created on: 2016¦~12¤ë23¤é
 *      Author: Administrator
 */
#include "usart.h"

void USARTSetBaudRate(USART_TypeDef *usart, int cpuRate, int baudRate){
	cpuRate *= 1000000;
	usart->BRR = (cpuRate + baudRate / 2) / baudRate;
}

void USARTSetWordLength(USART_TypeDef *usart, int bits){
	if(bits == 8){
		usart->CR1 &= ~USART_CR1_M1;
		usart->CR1 &= ~USART_CR1_M0;
	}else if(bits == 9){
		usart->CR1 &= ~USART_CR1_M1;
		usart->CR1 |= USART_CR1_M0;
	}else if(bits == 7){
		usart->CR1 |= USART_CR1_M1;
		usart->CR1 &= ~USART_CR1_M0;
	}
}

void USARTSetStopBit(USART_TypeDef *usart, int bits){
	//0 -> 1
	//1 -> 0.5
	//2 -> 2
	//3 -> 1.5
	usart->CR2 &= ~USART_CR2_STOP_Msk;
	usart->CR2 |= bits << USART_CR2_STOP_Pos;
}

void USARTEnableRXTX(USART_TypeDef *usart, int rx, int tx){
	usart->CR1 &= ~(3 << 2);
	usart->CR1 |= rx << USART_CR1_RE_Pos;
	usart->CR1 |= tx << USART_CR1_TE_Pos;
}

void USARTEnable(USART_TypeDef *usart){
	usart->CR1 |= USART_CR1_UE;
}

int USARTGetFlagStatus(USART_TypeDef *usart, int flag){
	return usart->ISR & flag;
}

void USARTSendData(USART_TypeDef *usart, int data){
	while(!USARTGetFlagStatus(usart, USART_ISR_TXE));
	usart->TDR = data & 0x1FF;
	while(!USARTGetFlagStatus(usart, USART_ISR_TC));
}

int USARTGetData(USART_TypeDef *usart){
	while(!USARTGetFlagStatus(usart, USART_ISR_RXNE));
	return usart->RDR & 0x1FF;
}

void USARTPrintString(USART_TypeDef *usart, const char *ptr){
	while(*ptr){
		USARTSendData(usart, *(ptr++));
	}
}

int USARTGetLine(USART_TypeDef *usart, char *ptr, int len){
	len--;
	char x;
	for(int i=0;i<len;i++){
		x = USARTGetData(usart);
		if(x == '\n' || x == '\r'){
			ptr[i] = 0;
			//USARTPrintString(usart, "\r\n");
			return i;
		}else if(x == 127){
			if(i){
				i--;
				//USARTSendData(usart, x);
			}
			i--;
		}else{
			//USARTSendData(usart, x);
			ptr[i] = x;
		}
	}
	return len;
}
