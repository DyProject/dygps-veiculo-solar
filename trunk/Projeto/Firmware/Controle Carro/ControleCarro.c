/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Carro                               *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.2                                       *
****************************************************************/
#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "direcao_def.h"
#include "globals_def.h"
#include "usart_def.h"
#include "adc_def.h"

#include <util/delay.h>

//----------------------------------------------------------------------------

BufferRecep bufferRX_g;

//----------------------------------------------------------------------------

void ValoresIniciaisBuffer();
void SolicitaReenvioDados();

//----------------------------------------------------------------------------

ISR(USART_RX_vect)							
{
	RecebeProtocolo(&bufferRX_g);
	if(bufferRX_g.completo == 'y' && bufferRX_g.iniciado == 'n') {
		DirecaoCarro(&bufferRX_g);
		TransmitiBuffer(&bufferRX_g.fonteAlimentacao);
	}	
}	

//----------------------------------------------------------------------------

ISR(TIMER0_OVF_vect)
{
	static uint8_t tempoRecep = 0;
	
	/*aproximadamente 3s, sen�o receber dados nesse tempo envia novamente os dados*/
	if(tempoRecep > 100) {
		SolicitaReenvioDados();
		tempoRecep = 0;
	}
	
	tempoRecep++;
}

//----------------------------------------------------------------------------

void SolicitaReenvioDados()
{
	/*Para a fun��o recebe protocolo*/
	//tracebufferRX_g.iniciado = 'n';
	//tracebufferRX_g.qntdDadosLido = 0;
	
	/*Para o carro*/
	bufferRX_g.estadoCarro = PARADO;
	CarroParado();
	
	//traceTransmitiBuffer(&bufferRX_g.fonteAlimentacao);
}

//----------------------------------------------------------------------------
int main()
{
	Usart_Init(MYUBRR);
	ADC_Init();
	ValoresIniciaisBuffer();
	ConfiguracoesDirecaoInit();
	
	/*Contador Timer 0*/
	TIMSK0 = 0b00000001;
	/*Prescaler do Timer0*/
	TCCR0B = (1<<CS02) | (1<<CS00);
	
	sei();	
	while(1){}		
} 

//----------------------------------------------------------------------------

void ValoresIniciaisBuffer()
{
	bufferRX_g.dutyCicleM1 = 0;
	bufferRX_g.dutyCicleM2 = 0;
	bufferRX_g.qntdDadosLido = 0;
	bufferRX_g.iniciado = 'n';
	bufferRX_g.completo = 'y';
	bufferRX_g.fonteAlimentacao = 'B';
	bufferRX_g.direcao = 'P';
	bufferRX_g.estadoCarro = PARADO;
}	

//----------------------------------------------------------------------------