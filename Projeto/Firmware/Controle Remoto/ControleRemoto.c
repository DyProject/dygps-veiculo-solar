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

/*Interrupção do botão que seleciona qual fonte de alimentação deve ser usada. Quando o botão é pressionado seta bufferDados_g.botaoSelFontePress = 'y' e 
desabilita a interrupção. A interrupção é novamente habilitada após enviar os dados e mostrar valores no lcd.*/
ISR(INT0_vect)
{
	bufferDados_g.botaoSelFontePress = 'y';
	/*Desabilita a interrupção até enviar os dados*/
	EIMSK &= ~(1 << INT0);
	/*Limpa flag de solicitação de nova interrupção. evita o bounce*/ 
	EIFR = (1 << INTF0);
}	 
		
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
		/*Para eleminar o ruído*/
		static int contBounce = 0;
		if(contBounce > 4) {		
			/*Habilita a interrupção do botão*/
			EIFR = (1 << INTF0);
			EIMSK = (1<<INT0);
			contBounce = 0;
		}			
		contBounce++;
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
}	

//----------------------------------------------------------------------------

int main()
{	
	ValoresIniciaisBuffer();
	Usart_Init(MYUBRR);
	/*Prescaler do Timer0, usado para fazer uma leitura do ADC.*/
	TCCR0B = (1<<CS02) | (1<<CS00);
	
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	EICRA = (1<<ISC00) ;  
	
	inic_LCD_4bits();					
	ADC_Init();
	sei();
		
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
	bufferDados_g.botaoSelFontePress = 'n';
}	

//----------------------------------------------------------------------------
