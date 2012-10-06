/************************************************************************************
*                               Projeto Integrador II                               *
*                          Acionamento Motores com Arduino                          *
*                  Alunos: Dyego Campos e Gustavo Pereira de Souza                  *
*                           Data: 04/10/2012 - Versão 1.0                           *
************************************************************************************/

#include <avr/io.h>
#include <util/delay.h>

/* DEFINIÇÕES DE MACROS */
#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))	//ATIVA O BIT X DA VARIÁVEL Y (COLOCA EM 1)
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))	//LIMPA O BIT X DA VARIÁVEL Y (COLOCA EM 0) 
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))	//TESTA O BIT X DA VARIÁVEL Y (RETORNA 0 OU 1)
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))	//COMPLEMENTA O BIT X DA VARIÁVEL Y (COLOCA EM 1 SE ESTAVA EM 0 E VICE-VERSA)

#define IN1	PB0
#define IN2	PB1
#define IN3	PB2
#define IN4	PB3
#define ENA	PB4
#define ENB	PB5

#define B_FRENTE	PD0
#define B_TRAZ		PD1
#define B_PARA		PD2
#define B_DIREITA	PD3
#define B_ESQUERDA	PD4
#define B_RETO		PD5
#define B_ENABLE	PD6


void anda_frente (void);
void anda_atras (void);
void para (void);
void vira_direita(void);
void vira_esquerda(void);
void anda_reto(void);

int main(void)
{
    DDRB	= 0xFF;			//Saídas para acionamento L298
	PORTB	= 0x00;			//Saídas do PORTB com nível lógico baixo
	DDRD	= 0x00;			//Botões de acionamento
	PORTD	= 0xFF;			//Pull-up habilitado
		
	while(1)
	{
		while(tst_bit(PIND, B_ENABLE));
		set_bit(PORTB, ENA);
		set_bit(PORTB, ENB);		
		while(!tst_bit(PIND, B_ENABLE))
		{
			if(!tst_bit(PIND, B_FRENTE))
				anda_frente();
			if(!tst_bit(PIND, B_TRAZ))
				anda_atras();
			if(!tst_bit(PIND, B_PARA))
				para();
			if(!tst_bit(PIND, B_DIREITA))
				vira_direita();
			if(!tst_bit(PIND, B_ESQUERDA))
				vira_esquerda();
			if(!tst_bit(PIND, B_RETO))
				anda_reto();
		}
		clr_bit(PORTB, ENA);
		clr_bit(PORTB, ENB);		
	}	
}

void anda_frente (void)
{
	para();
	_delay_ms(100);	
	clr_bit(PORTB, IN1);
	set_bit(PORTB, IN2);
	clr_bit(PORTB, IN3);
	set_bit(PORTB, IN4);
}
void anda_atras (void)
{
	para();
	_delay_ms(100);
	set_bit(PORTB, IN1);
	clr_bit(PORTB, IN2);
	set_bit(PORTB, IN3);
	clr_bit(PORTB, IN4);
}
void para (void)
{
	set_bit(PORTB, IN1);
	set_bit(PORTB, IN2);
	set_bit(PORTB, IN3);
	set_bit(PORTB, IN4);
}
void vira_direita(void)
{
	clr_bit(PORTB, ENA);
	set_bit(PORTB, ENB);
}
void vira_esquerda(void)
{
	set_bit(PORTB, ENA);
	clr_bit(PORTB, ENB);
}
void anda_reto(void)
{
	set_bit(PORTB, ENA);
	set_bit(PORTB, ENB);
}