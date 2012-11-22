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

#include "LCD.h"
#include "macro_bit.h"
#include "spidef.h"

//--------------------------------------------------------------

int main(void)
{
	/* desabilita os pinos de RX e TX */
	UCSR0B=0x00;
				
	/* Para uso do botão */
	DDRB &= (~(1 << PB0));
	PORTB |= (1 << PB0);
	
	LCD_inicia_4bits();
	
	SPI_MasterInit();
		
    while(1)
    {
		if(!tst_bit(PINB, PB0)) {
			SPI_HABILITA_SLAVE();	
			_delay_ms(20);
			SPI_Transmit('A');
			SPI_DESABILITA_SLAVE();
			while(!tst_bit(PINB, PB0));
		}		
    }
}

//--------------------------------------------------------------
