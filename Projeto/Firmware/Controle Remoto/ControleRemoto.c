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

volatile uint8_t podeIniciarNovaTrasmissao = 1;
BufferDados bufferDados_g;

//----------------------------------------------------------------------------

void ValoresIniciaisBuffer();

//----------------------------------------------------------------------------

ISR(ADC_vect)			
{
	static uint8_t contador = 0;
		
	/*Envia o valor lido do ADC a cada 1s aproximadamente*/
	if(contador > 16 && (bufferDados_g.podeIniciarTransmissao == 'y')) {
		ADMUX &= ~(1 << ADIE);
			
		TransmitiBuffer(&bufferDados_g);
		//Limpa_matriz_LCD(1, 6, 1);
		//Limpa_matriz_LCD(2, 6, 1);
		MostraDadosLCD(&bufferDados_g);
		bufferDados_g.podeIniciarTransmissao = 'n';
		contador = 0;
		
		ADMUX |= (1 << ADIE);
	}		
	
	contador++;
	/*Limpa o flag de overflow do Timer0. Esse flag indica que houve um estouro do timer.
	limpar para habilitar um novo estouro para gerar a interrupção do ADC.*/
	TIFR0 |= TOV0;
}	

//----------------------------------------------------------------------------

ISR(USART_RX_vect)							
{	
	RecebeProtocolo(&bufferDados_g);
	if(bufferDados_g.completo == 'y') 
		bufferDados_g.podeIniciarTransmissao = 'y';
	
	/*uint16_t recebido = UDR0;
	
	/*Recebe a letra 'z' indicando que o controle carro recebeu os dados*//*
	if (recebido == 'z')
		podeIniciarNovaTrasmissao = 1;*/
}	

//----------------------------------------------------------------------------

int main()
{	
	ValoresIniciaisBuffer();
	Usart_Init(MYUBRR);
	/*Prescaler do Timer0, usado para fazer uma leitura do ADC.*/
	TCCR0B = (1<<CS02) | (1<<CS00);
	inic_LCD_4bits();					
	ADC_Init();
	sei();
	
	/*Para teste*/		
	TransmitiBuffer(&bufferDados_g);
	//Limpa_matriz_LCD(1, 6, 1);
	//Limpa_matriz_LCD(2, 6, 1);
	//MostraDadosLCD(&bufferDados_g);
	
	while(1){}
}

//----------------------------------------------------------------------------

void ValoresIniciaisBuffer()
{
	bufferDados_g.qntdDadosLido = 0;
	bufferDados_g.iniciado = 'n';
	bufferDados_g.completo = 'n';
	bufferDados_g.podeIniciarTransmissao = 'y';
	bufferDados_g.fonteAlimentacao = 'B';
}	

//----------------------------------------------------------------------------
