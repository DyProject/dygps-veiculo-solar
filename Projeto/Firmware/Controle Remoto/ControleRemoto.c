/*!**************************************************************
*\mainpage		Solar Foot 
*\date			07/08/2015
*\author		Dyego de Campos
*\author		Daniel Dezan de Bona
*
*\brief	Solar Foot ser� utilizado como modelo na disciplina de PI2 do CST de Sistemas Eletr�nicos.
*		A ideia � realizar uma partida de futebol com os carros do projeto.
*	
*Solar Foot � baseado no Projeto DYGPS (PI2) do CST em Sistemas Eletr�nicos de:
*	-# CAMPOS, Dyego de</li>                        
*	-# SOUZA, Gustavo Pereira de
*                                   
****************************************************************/
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "controle_def.h"
#include "globals_def.h"
#include "usart_def.h"
#include "adc_def.h"
#include "LCD4b.h"
#include "stdio.h"
#include "adc_def.h"
#include "joystick.h"

#include <util/delay.h>

BufferDados bufferDados_g; /*!<Armazena as principais informa��es que ser�o transmitidas ou que ser�o utilizadas para calcular os valores a transmitir.*/

//----------------------------------------------------------------------------

/*!
	\fn void ValoresIniciaisBuffer()
	\param void
	\return void
	\brief Seta os valores inicias dos dados.
*/
void ValoresIniciaisBuffer();

//----------------------------------------------------------------------------

/*!\fn ISR(INT0_vect)
	\brief	Interrup��o do bot�o que seleciona qual fonte de alimenta��o deve ser usada. 
*/
ISR(INT0_vect)
{
	bufferDados_g.botaoSelFontePress = 'y';
	
	/*! - EIMSK &= ~(1 << INT0) - Desabilita a interrup��o at� enviar os dados e estes serem apresentados no lcd*/
	EIMSK &= ~(1 << INT0);
	
	/*! - EIFR = (1 << INTF0) - Limpa flag de solicita��o de nova interrup��o. evita o bounce*/ 
	EIFR = (1 << INTF0);
}	 
		
//----------------------------------------------------------------------------

/*!\fn ISR(ADC_vect)	
	\brief	Interrup��o do AD gerada pelo Overflow do Timer 0.
			
			f_estouro = Nr_max_con * T * Prescaler
				Sendo:
					- Nr_max_con - 256 para o Timer 0 que � de 8 bits;
					- T - Per�odo 1/F_CPU;
					- Prescaler - Definido nos CS02, CS01 e CS00 do registrador TCCR0B;
					
			Para o prescaler de 1024 f_estouro = 16,384 ms 
*/
ISR(ADC_vect)			
{
	static uint8_t contador = 0;
	if(contador > 10 && (bufferDados_g.podeIniciarTransmissao == 'y')) {
		
		/*Desabilita Interrup��o RX*/
		clr_bit(UCSR0B, 7);
		
		/*Desabilita AD*/
		ADMUX &= ~(1 << ADIE);
			
		TransmitiBuffer(&bufferDados_g);
		MostraDadosLCD(&bufferDados_g);
		
		/*Bounce - Para eleminar o ru�do*/
		static int contBounce = 0;
		if(contBounce > 4) {		
			/*Habilita a interrup��o do bot�o*/
			EIFR = (1 << INTF0);
			EIMSK = (1<<INT0);
			contBounce = 0;
		}			
		contBounce++;
		
		bufferDados_g.podeIniciarTransmissao = 'n';
		contador = 0;
		
		/*Habilita AD*/
		ADMUX |= (1 << ADIE);
		
		/*Habilita Interrup��o RX*/
		set_bit(UCSR0B, 7);
		
		/*Indica se h� comunica��o*/
		bufferDados_g.estacomunicando = 's';
		
	} else if((contador > 15 && !bufferDados_g.iniciado == 'n') || contador > 100) {
		/*Desabilita Interrup��o RX*/
		clr_bit(UCSR0B, 7);
		
		ADMUX &= ~(1 << ADIE);
		TransmitiBuffer(&bufferDados_g);
		/*Habilita Interrup��o RX*/
		set_bit(UCSR0B, 7);
		
		/*Indica se h� comunica��o*/
		bufferDados_g.estacomunicando = 'n';
		
		MostraDadosLCD(&bufferDados_g);
		contador = 0;
		
		ADMUX |= (1 << ADIE);
	} 	
	
	contador++;
	/*!TIFR0 |= TOV0 - Limpa o flag de overflow do Timer0. Esse flag indica que houve um estouro do timer.
	limpar para habilitar um novo estouro para gerar a interrup��o do ADC.*/
	TIFR0 |= TOV0;
	
}	

//----------------------------------------------------------------------------

/*!\fn ISR(USART_RX_vect)	
	\brief	Interrup��o da comunica��o serial RX.
*/
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
	
	/*! \brief	Configura��o do prescaler do Timer 0. Ele est� sendo usado para fazer leitura do ADC. 
				A cada overflow gera uma interrup��o.
	
				*Prescaler do Timer0 \n
				*2      1		0		(CSx bits)\n
				*0		0		0	-	No clock source (Timer/Counter stopped) \n
				*0		0		1	-	clk I/O /(No prescaling) \n
				*0		1		0	-	clk I/O /8 (From prescaler)\n
				*0		1		1	-	clk I/O /64 (From prescaler)\n
				*1		0		0	-	clk I/O /256 (From prescaler)\n
				*1		0		1	-	clk I/O /1024 (From prescaler)\n
				*1		1		0	-	External clock source on T0 pin. Clock on falling edge.\n
				*1		1		1	-	External clock source on T0 pin. Clock on rising edge.\n
	*/
	TCCR0B |= (1<<CS02) | (1<<CS00);
	
	DDRD &= ~(1 << DDD2); //PD2 como entrada
	PORTD |= (1 << DDD2);//Habilita pull up interno
	DDRD &= ~(1 << DDD3);
	PORTD |= (1 << DDD3);
	
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
	bufferDados_g.completo = 'y';
	bufferDados_g.podeIniciarTransmissao = 'y';
	bufferDados_g.fonteAlimentacao = 'B';
	bufferDados_g.botaoSelFontePress = 'n';
	bufferDados_g.estacomunicando = 'n';
}	

//----------------------------------------------------------------------------
