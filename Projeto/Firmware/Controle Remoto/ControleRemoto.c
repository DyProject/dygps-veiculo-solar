/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Controle Remoto                              *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.1                                       *
****************************************************************/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//----------------------------------------------------------------------------

#include "controle_def.h"
#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

void Inicializacoes();

void MostraDadosLCD(
	uint16_t dutyLadoEsq, 
	uint16_t dutyLadoDir, 
	unsigned char direcao
);
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
	if(contador == 32) {	
		unsigned char sentido;	
		uint8_t dutyLadorEsq;
		uint8_t dutyLadorDir;
		
		dutyLadorEsq = CalculaDutyCycleLadoEsq();
		dutyLadorDir= CalculaDutyCycleLadoDir();
		
		sentido = CalculaSentido();
		
		TransmitiBuffer(dutyLadorEsq, dutyLadorDir, sentido);
		MostraDadosLCD(dutyLadorEsq, dutyLadorDir, sentido);
						
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
	
	
	inic_LCD_4bits();					
	ADC_Init();
	sei();
}

//----------------------------------------------------------------------------

void MostraDadosLCD(
	uint16_t dutyLadoEsq, 
	uint16_t dutyLadoDir, 
	unsigned char direcao
)
{
	LCD_setPos(1,0);
	EscreveCaracterLCD('[');	
	EscreveCaracterLCD(direcao);
	EscreveCaracterLCD(']');	
	
	EscreveCaracterLCD('[');
	unsigned char caracConvertido[5];
	CvrtNum2CaracterIndividual(dutyLadoEsq, caracConvertido);
	escreve_LCD(caracConvertido);
	
	CvrtNum2CaracterIndividual(dutyLadoDir, caracConvertido);
	escreve_LCD(caracConvertido);
	
	EscreveCaracterLCD(']');
}

//----------------------------------------------------------------------------

