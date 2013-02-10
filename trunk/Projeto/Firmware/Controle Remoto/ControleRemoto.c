/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Remoto                              *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.1                                       *
****************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

//----------------------------------------------------------------------------

#include "controle_def.h"
#include "macros_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

extern volatile TEstadoCarro estadoCarro;

//----------------------------------------------------------------------------
 
 ISR(PCINT0_vect)
{
	//Desabilita interrupção PCINT[7..0]
	PCICR	&= (~(1 << PCIE0));
	PCMSK0 = 0x00;
	
	EnviaDirecaoCarro();
	
	_delay_ms(50);
	PCMSK0	|= (1 << BT_FRENTE) | (1 << BT_RE)  | (1 << BT_DIREITA) | (1 << BT_ESQUERDA);
	PCICR	|= (1 << PCIE0);
}

//----------------------------------------------------------------------------

int main()
{	
	DDRB	= 0x00;			
	PORTB	= 0x0F;	
	
	//Habilita interrupção PCINT[7..0]
	PCICR	|= (1 << PCIE0);
	//Habilita interrupção nos pino PCINT
	PCMSK0	|= (1 << BT_FRENTE) | (1 << BT_RE)  | (1 << BT_DIREITA) | (1 << BT_ESQUERDA);		
		
	Usart_Init(MYUBRR);
	//Para garantir que o carro sempre inicie parado.
	Usart_Transmit('P');
	sei();
		
	while(1){		
	}
}

//----------------------------------------------------------------------------

