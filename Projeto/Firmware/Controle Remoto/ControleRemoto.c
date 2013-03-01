/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Remoto                              *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.1                                       *
****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "controle_def.h"
#include "globals_def.h"
#include "usart_def.h"

#include <util/delay.h>

//----------------------------------------------------------------------------

ISR(ADC_vect)			
{
	Protocolo();
}	

//----------------------------------------------------------------------------

int main()
{	
	Inicializacoes();
	sei();
	while(1){}
}

//----------------------------------------------------------------------------


