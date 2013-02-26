/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Remoto                              *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.1                                       *
****************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

//----------------------------------------------------------------------------

#include "controle_def.h"
#include "macros_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

void Inicializacoes();

//----------------------------------------------------------------------------

int main()
{	
	Inicializacoes();
	
	while(1){}
}

//----------------------------------------------------------------------------

ISR(ADC_vect)			
{
	static uint8_t contador = 0;
		
	/*Envia o valor lido do ADC a cada 1s aproximadamente*/
	if(contador == 64) {	
		unsigned char direcao;
		unsigned char sentido;	
		uint16_t dutyLadorEsq;
		uint16_t dutyLadorDir;
		
		dutyLadorEsq = CalculaDutyCycleLadoEsq();
		dutyLadorDir= CalculaDutyCycleLadoDir();
		
		sentido = CalculaSentido();
		direcao = DirecaoCarro(sentido);
		
		TransmitiBuffer(dutyLadorEsq, dutyLadorDir, direcao);
						
		contador = 0;
	}		
	
	contador++;
	
	/*Limpa o flag de overflow do Timer0. Esse flag indica que houve um estouro do timer.
	limpar para habilitar um novo estouro para gerar a interrupção do ADC.*/
	TIFR0 |= TOV0;
}	

//----------------------------------------------------------------------------

void Inicializacoes()
{
	Usart_Init(MYUBRR);
	
	//Prescaler do Timer0, usado para fazer uma leitura do ADC.
	TCCR0B = (1<<CS02) | (1<<CS00);
	ADC_Init();
	
	sei();
}

//----------------------------------------------------------------------------

