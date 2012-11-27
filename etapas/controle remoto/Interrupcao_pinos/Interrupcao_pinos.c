/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title:		Interrupt PORTs                         *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:		27/11/2012                              *
*           Version:                                            *
****************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000

#define cpl_bit(adress,bit) (adress ^= (1<<bit))
#define tst_bit(adress,bit) (adress & (1<<bit))   

ISR(PCINT0_vect)
{
	PCICR	&= (~(1 << PCIE0));
	cpl_bit(PORTC, PC0);
	while(tst_bit(PINB, PB0));
	PCICR	|= (1 << PCIE0);
}


int main(void)
{
    DDRB	&= (~(1 << PB0));
	PORTB	|= (1 << PB0);
	DDRC	|= (1 << PC0);
	
	PCICR	|= (1 << PCIE0);
	PCMSK0	|= (1 << PCINT0);
	
	sei();
	
	while(1)
    { }
}