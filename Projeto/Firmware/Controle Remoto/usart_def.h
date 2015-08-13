#ifndef _USART_H
#define _USART_H

#include <avr/io.h> 	    
#include "stdio.h"

//---------------------------------------------------------------------------

/* Baud Rate:
* Modo Assincrono:			BAUD = (fosc) / (16*(UBRRN + 1))
* Modo Sincrono 2x speed	BAUD = (fosc) / (8 * (UBRRN + 1))
* Modo Sincrono MAster		BAUD = (fosc) / (2 * (UBRRN + 1))
*/  	
#define BAUD 	9600	
#define MYUBRR	F_CPU/16/BAUD-1

//---------------------------------------------------------------------------

void Usart_Init(unsigned int);
void Usart_Transmit(unsigned char );
void Usart_Transmit_Printf(char dado, FILE *stream);
unsigned char Usart_Receive();
void Usart_Write(char*);
void Usart_Write_Flash(const char*);
//---------------------------------------------------------------------------

#endif