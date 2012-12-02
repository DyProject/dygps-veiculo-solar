/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title:		Example USART Communications            *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:       02/12/2012                              *
*           Version:                                            *
****************************************************************/

#define F_CPU 1600000UL

//----------------------------------------------------------------------

#include <avr/interrupt.h>
#include <avr/io.h>

#include "usart_def.h"

//----------------------------------------------------------------------

int main(void)
{
	Usart_Init(MYUBRR);
	
	sei();
	
	Usart_transmit("DYGPS Veiculo Solar\0");
	
    while(1)
    {
        
    }
}

//----------------------------------------------------------------------
