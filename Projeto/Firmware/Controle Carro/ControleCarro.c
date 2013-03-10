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

volatile BufferRecep* bufferRX_g;

//----------------------------------------------------------------------------

int recebido = 0;

ISR(USART_RX_vect)							
{
	UCSR0B &= ~(1 << RXCIE0);
	RecebeProtocolo(bufferRX_g);
	if(bufferRX_g->completo == 'y') {
		TrasmitiBuffer(bufferRX_g);	
	}				
	UCSR0B |= (1 << RXCIE0);
}	

//----------------------------------------------------------------------------

int main()
{
	ConfiguracoesDirecaoInit(bufferRX_g);
	Usart_Init(MYUBRR);
	ADC_Init();
	sei();
			
	while(1){}		
} 

