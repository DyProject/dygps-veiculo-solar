/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title:		PWM                                     *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:		01/11/2012                              *
*           Version:	                                        *
****************************************************************/

#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>

#include "LCD.h"

//--------------------------------------------------------------

#define F_CPU 10000000UL

#define  clr_bit(adress,bit)  (adress&=~(1<<bit))
#define  set_bit(adress,bit)  (adress|=(1<<bit)) 
#define tst_bit(adress,bit)  (adress&(1<<bit)) 

#define FATOR_DT 0.390625
#define FATOR_COMP 2.56

//--------------------------------------------------------------

void DutyCicle(unsigned int);

//--------------------------------------------------------------

volatile unsigned int valorComp = 210;

//--------------------------------------------------------------

int main(void)
{
	DDRD = 0b11111101;
	PORTD = 0xFF;
	DDRB = 0xFF;
	
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00); //Fast PWM
	TCCR0B = (1 << CS01) | (1 << CS00); 
		
	TIMSK0 = (1 << TOIE0);
	
	LCD_inicia_4bits();
	
	char texto[6] = "dygps - Veiculo Solar";
	
	LCD_escreve(texto);
	
	sei();
	
	int valorDuty = 0;
	
    while(1)
    {
		if(!tst_bit(PIND, PIND1)){
			_delay_ms(10);
			DutyCicle(valorDuty);
			valorDuty += 10;

			while(!tst_bit(PIND, PIND1)){}
			_delay_ms(10);
		}			
    }
	return 0;
}

//--------------------------------------------------------------

ISR(TIMER0_OVF_vect)
{
	static int cont = 0;
	static int duty = 10;
	OCR0A = valorComp;	
	
	
	cont++;
	if (cont == 1000){
		DutyCicle(duty);
		duty +=10;
	}			
}

//--------------------------------------------------------------

void DutyCicle(unsigned int duty)
{
	char buffer[5];
	valorComp = floor(FATOR_COMP * duty);
	itoa(valorComp, buffer, 10);
	LCD_escreve(buffer);
}

//--------------------------------------------------------------

//--------------------------------------------------------------