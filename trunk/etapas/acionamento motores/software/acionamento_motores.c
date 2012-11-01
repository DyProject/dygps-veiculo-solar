/************************************************************************************
*                               Projeto Integrador II                               *
*                          Acionamento Motores com Arduino                          *
*                  Alunos: Dyego Campos e Gustavo Pereira de Souza                  *
*                           Data: 04/10/2012 - Versão 1.0                           *
************************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL


#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))

#define ENA	PC0
#define IN1	PC1
#define	IN2	PC2
#define ENB	PC3
#define IN3	PC4
#define IN4	PC5 


#define B_FORWARD	PD0
#define B_BACKWARD	PD1
#define B_STOP		PD2
#define B_RIGHT		PD3
#define B_LEFT		PD4
#define B_MOVE_ON	PD5
#define B_ENABLE	PD6


void FORWARD();
void BACKWARD();
void STOP();
void RIGHT();
void LEFT();
void MOVE_ON();

int main()
{
    DDRC	= 0xFF;			
	PORTC	= 0x00;			
	DDRD	= 0x00;			
	PORTD	= 0xFF;	
	
	UCSR0B	= 0x00;
			
	while(1)
	{
		while(tst_bit(PIND, B_ENABLE));
		_delay_ms(10);
		set_bit(PORTC, ENA);
		set_bit(PORTC, ENB);		
		while(!tst_bit(PIND, B_ENABLE))
		{
			if(!tst_bit(PIND, B_FORWARD))
			{
				_delay_ms(10);
				FORWARD();		
				while(!tst_bit(PIND, B_FORWARD));
				_delay_ms(10);		
			}			
			if(!tst_bit(PIND, B_BACKWARD))
			{
				_delay_ms(10);
				BACKWARD();
				while(!tst_bit(PIND, B_BACKWARD));	
				_delay_ms(10);			
			}				
			if(!tst_bit(PIND, B_STOP))
			{
				_delay_ms(10);
				STOP();
				while(!tst_bit(PIND, B_STOP));		
				_delay_ms(10);		
			}				
			if(!tst_bit(PIND, B_RIGHT))
			{
				_delay_ms(10);
				RIGHT();
				while(!tst_bit(PIND, B_RIGHT));		
				_delay_ms(10);		
			}				
			if(!tst_bit(PIND, B_LEFT))
			{
				_delay_ms(10);
				LEFT();
				while(!tst_bit(PIND, B_LEFT));		
				_delay_ms(10);		
			}				
			if(!tst_bit(PIND, B_MOVE_ON))
			{
				_delay_ms(10);
				MOVE_ON();
				while(!tst_bit(PIND, B_MOVE_ON));		
				_delay_ms(10);		
			}				
		}
		_delay_ms(10);
		clr_bit(PORTC, ENA);
		clr_bit(PORTC, ENB);		
	}	
} /*MAIN*/

void FORWARD()
{
	STOP();
	clr_bit(PORTC, IN1);
	set_bit(PORTC, IN2);
	clr_bit(PORTC, IN3);
	set_bit(PORTC, IN4);
}
void BACKWARD()
{
	STOP();	
	set_bit(PORTC, IN1);
	clr_bit(PORTC, IN2);
	set_bit(PORTC, IN3);
	clr_bit(PORTC, IN4);
}
void STOP()
{
	set_bit(PORTC, IN1);
	set_bit(PORTC, IN2);
	set_bit(PORTC, IN3);
	set_bit(PORTC, IN4);
	_delay_ms(100);
}
void RIGHT()
{
	clr_bit(PORTC, ENA);
	set_bit(PORTC, ENB);
}
void LEFT()
{
	set_bit(PORTC, ENA);
	clr_bit(PORTC, ENB);
}
void MOVE_ON()
{
	set_bit(PORTC, ENA);
	set_bit(PORTC, ENB);
}