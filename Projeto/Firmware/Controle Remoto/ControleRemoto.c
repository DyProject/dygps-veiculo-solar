/*!**************************************************************
*\mainpage		Solar Foot 
*\date			07/08/2015
*\author		Dyego de Campos
*\author		Daniel Dezan de Bona
*
*\brief	Solar Foot será utilizado como modelo na disciplina de PI2 do CST de Sistemas Eletrônicos.
*		A ideia é realizar uma partida de futebol com os carros do projeto.
*	
*Solar Foot é baseado no Projeto DYGPS (PI2) do CST em Sistemas Eletrônicos de:
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

BufferDados bufferDados_g; /*!<Armazena as principais informações que serão transmitidas ou que serão utilizadas para calcular os valores a transmitir.*/

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
	\brief	Interrupção do botão que seleciona qual fonte de alimentação deve ser usada. 
*/
ISR(INT0_vect)
{
	bufferDados_g.botaoSelFontePress = 'y';
	
	/*! - EIMSK &= ~(1 << INT0) - Desabilita a interrupção até enviar os dados e estes serem apresentados no lcd*/
	EIMSK &= ~(1 << INT0);
	
	/*! - EIFR = (1 << INTF0) - Limpa flag de solicitação de nova interrupção. evita o bounce*/ 
	EIFR = (1 << INTF0);
}	 
		
//----------------------------------------------------------------------------

/*!\fn ISR(ADC_vect)	
	\brief	Interrupção do AD gerada pelo Overflow do Timer 0.
			
			f_estouro = Nr_max_con * T * Prescaler
				Sendo:
					- Nr_max_con - 256 para o Timer 0 que é de 8 bits;
					- T - Período 1/F_CPU;
					- Prescaler - Definido nos CS02, CS01 e CS00 do registrador TCCR0B;
					
			Para o prescaler de 1024 f_estouro = 16,384 ms 
*/
ISR(ADC_vect)			
{
	static uint8_t contador = 0;
	if(contador > 10 && (bufferDados_g.podeIniciarTransmissao == 'y')) {
		
		/*Desabilita Interrupção RX*/
		clr_bit(UCSR0B, 7);
		
		/*Desabilita AD*/
		ADMUX &= ~(1 << ADIE);
			
		TransmitiBuffer(&bufferDados_g);
		MostraDadosLCD(&bufferDados_g);
		
		/*Bounce - Para eleminar o ruído*/
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
		
		/*Habilita AD*/
		ADMUX |= (1 << ADIE);
		
		/*Habilita Interrupção RX*/
		set_bit(UCSR0B, 7);
		
		/*Indica se há comunicação*/
		bufferDados_g.estacomunicando = 's';
		
	} else if((contador > 15 && !bufferDados_g.iniciado == 'n') || contador > 100) {
		/*Desabilita Interrupção RX*/
		clr_bit(UCSR0B, 7);
		
		ADMUX &= ~(1 << ADIE);
		TransmitiBuffer(&bufferDados_g);
		/*Habilita Interrupção RX*/
		set_bit(UCSR0B, 7);
		
		/*Indica se há comunicação*/
		bufferDados_g.estacomunicando = 'n';
		
		MostraDadosLCD(&bufferDados_g);
		contador = 0;
		
		ADMUX |= (1 << ADIE);
	} 	
	
	contador++;
	/*!TIFR0 |= TOV0 - Limpa o flag de overflow do Timer0. Esse flag indica que houve um estouro do timer.
	limpar para habilitar um novo estouro para gerar a interrupção do ADC.*/
	TIFR0 |= TOV0;
	
}	

//----------------------------------------------------------------------------

/*!\fn ISR(USART_RX_vect)	
	\brief	Interrupção da comunicação serial RX.
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
	
	/*! \brief	Configuração do prescaler do Timer 0. Ele está sendo usado para fazer leitura do ADC. 
				A cada overflow gera uma interrupção.
	
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
