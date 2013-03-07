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

//----------------------------------------------------------------------------

ISR(ADC_vect)			
{
	static uint8_t contador = 0;
		
	/*Envia o valor lido do ADC a cada 1s aproximadamente*/
	if(contador > 16 && podeIniciarNovaTrasmissao) {
		ADMUX &= ~(1 << ADIE);
			
		Protocolo();
		podeIniciarNovaTrasmissao = 0;
		
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
	uint8_t mostrarLcd = RecebeProtocolo();
	if(mostrarLcd) {
		/*Desabilita AD*/
		ADMUX &= ~(1 << ADIE);
		podeIniciarNovaTrasmissao = 1;
		MostraDadosLCD();
		ADMUX |= (1 << ADIE);
	}	
}	

//----------------------------------------------------------------------------

int main()
{	
	Inicializacoes();
	Limpa_matriz_LCD(1, 0, 16);
	Limpa_matriz_LCD(2, 0, 16);
	LCD_setPos(1, 2);
	escreve_LCD("DYGPS - 2013");
	sei();
	while(1){}
}

//----------------------------------------------------------------------------


