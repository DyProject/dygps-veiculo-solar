/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Carro                               *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.2                                       *
****************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL

#include "usart_def.h"

//----------------------------------------------------------------------------

#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))

#define ENA		PC0
#define IN1_IN3	PC1
#define	IN2_IN4	PC2
#define ENB		PC3

//----------------------------------------------------------------------------

unsigned char direcao;

//----------------------------------------------------------------------------

int main()
{
    DDRC	= 0xFF;			
	PORTC	= 0x00;	
	
	Usart_Init(MYUBRR);
			
	while(1)
	{
		direcao = Usart_Receive();
		
		if(direcao == 'F')
		{
			set_bit(PORTC, IN1_IN3);	//Liga motores para frente
			clr_bit(PORTC, IN2_IN4);	//
			set_bit(PORTC, ENA);
			set_bit(PORTC, ENB);
		}else
		if(direcao == 'R')
		{
			set_bit(PORTC, IN2_IN4);	//Liga motores para ré
			clr_bit(PORTC, IN1_IN3);	//
			set_bit(PORTC, ENA);
			set_bit(PORTC, ENB);
		}else
		if(direcao == 'P')
		{
			clr_bit(PORTC, IN2_IN4);	//Para motores
			clr_bit(PORTC, IN1_IN3);	//
			set_bit(PORTC, ENA);
			set_bit(PORTC, ENB);
		}
			
		if(direcao == 'D')
		{
			set_bit(PORTC, ENA);	//Para motor da direita
			clr_bit(PORTC, ENB);	//
		}else
		if(direcao == 'E')
		{
			clr_bit(PORTC, ENA);	//Para motor da esquerda
			set_bit(PORTC, ENB);	//
		}else
		if(direcao == 'C')
		{
			set_bit(PORTC, ENA);	//
			set_bit(PORTC, ENB);	//Liga os dois motores
		}
	}		
} /*MAIN*/