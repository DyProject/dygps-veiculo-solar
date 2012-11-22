/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title:		SPI communication                       *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:		22/11/2012                              *
*           Version:                                            *
****************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "LCD.h"
#include "spidef.h"

//--------------------------------------------------------------

#define F_CPU 8000000UL

//--------------------------------------------------------------

volatile char dadoRecebido;
volatile char podeImprimir = 0;

//--------------------------------------------------------------

ISR(SPI_STC_vect)
 {	 
	dadoRecebido = SPDR;
	podeImprimir = 1;	
}

//--------------------------------------------------------------

int main(void)
{
	UCSR0B=0x00;	//desabilita os pinos de RX e TX
	
	SPI_SlaveInit();
	LCD_inicia_4bits();

	SPI_SlaveInit();
	
	SPI_HABILITA_INTERRUPCAO_SPI();
	sei();

    while(1)
    {		
		if(podeImprimir) {
			LCD_comando(dadoRecebido, 1);
			podeImprimir = 0;
		}					
    }
}

//--------------------------------------------------------------