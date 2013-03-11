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

#include <util/delay.h>

//----------------------------------------------------------------------------

BufferRecep bufferRX_g;

//----------------------------------------------------------------------------

void ValoresIniciaisBuffer();

//----------------------------------------------------------------------------

ISR(USART_RX_vect)							
{
	RecebeProtocolo(&bufferRX_g);
	if(bufferRX_g.completo == 'y') {
		DirecaoCarro(&bufferRX_g);
		TransmitiBuffer(&bufferRX_g.fonteAlimentacao);
	}	
}	
	
//----------------------------------------------------------------------------
int main()
{
	Usart_Init(MYUBRR);
	ADC_Init();
	ValoresIniciaisBuffer();
	ConfiguracoesDirecaoInit();
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
	bufferRX_g.completo = 'n';
	bufferRX_g.fonteAlimentacao = 'B';
	bufferRX_g.direcao = 'P';
	bufferRX_g.estadoCarro = PARADO;
}	

//----------------------------------------------------------------------------