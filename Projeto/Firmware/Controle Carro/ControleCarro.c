/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Carro                               *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.2                                       *
****************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "direcao_def.h"
#include "globals_def.h"
#include "usart_def.h"

#include <util/delay.h>

//----------------------------------------------------------------------------

ISR(USART_RX_vect)							
{
	RecebeProtocolo();
}	

//----------------------------------------------------------------------------

int main()
{
	Usart_Init(MYUBRR);
	ConfiguracoesDirecaoInit();
	sei();
			
	while(1){}		
} 

