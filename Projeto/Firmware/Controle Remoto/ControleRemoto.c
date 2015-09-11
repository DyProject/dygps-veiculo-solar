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

unsigned char bufferTransmissao_g[QUANT_DADOS_PACOTE_TRANS];
unsigned char bufferRecepcao_g[QUANT_DADOS_PACOTE_TRANS];

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
//ISR(INT0_vect)
//{
	//bufferDados_g.botaoSelFontePress = 'y';
	//
	///*! - EIMSK &= ~(1 << INT0) - Desabilita a interrupção até enviar os dados e estes serem apresentados no lcd*/
	//EIMSK &= ~(1 << INT0);
	//
	///*! - EIFR = (1 << INTF0) - Limpa flag de solicitação de nova interrupção. evita o bounce*/ 
	//EIFR = (1 << INTF0);
//}	 
		//
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
	static uint8_t prescaler = 0;
		
	if(prescaler > 10) {
		if(bufferDados_g.dadosRecebidosComSucesso == 'y') {
			bufferDados_g.msgLCD4 = MSG_1;	
			CarregaBufferTransmissao(&bufferDados_g, bufferTransmissao_g);
			MostraDadosLCD(&bufferDados_g, bufferTransmissao_g);
			TransmitiBuffer(bufferTransmissao_g);
			bufferDados_g.dadosRecebidosComSucesso = 'n';
			contador = 0;
		}
		else if(contador > 100) { //Timeout
			bufferDados_g.msgLCD4 = MSG_INICIAL;	
			CarregaBufferTransmissao(&bufferDados_g, bufferTransmissao_g);
			TransmitiBuffer(bufferTransmissao_g);
			bufferDados_g.estacomunicando = 'n';
			MostraDadosLCD(&bufferDados_g, bufferTransmissao_g);
			contador = 0;
		}
		prescaler = 0;
	}
	prescaler++;
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
	/*PROVISORIO*/
	uint8_t dadoRecebido = UDR0;
	bufferDados_g.estacomunicando = 'y';
	bufferDados_g.dadosRecebidosComSucesso = 'y';
	
	/*IMPLEMENTANDO*/
	//static uint8_t contDados = 0;
	//uint8_t dadoRecebido = UDR0;
	//
	//if(dadoRecebido == ACK_RECEPTOR)
		//bufferDados_g.estadoBufferRecep = IDLE;
	//
	//switch(bufferDados_g.estadoBufferRecep) {
		//case IDLE:
		//if(dadoRecebido == ACK_RECEPTOR) {
			//bufferDados_g.estadoBufferRecep = INICIADO;
			//bufferDados_g.estacomunicando = 'y';
			//bufferDados_g.dadosRecebidosComSucesso = 'n';
		//}
		//else if (dadoRecebido == ERRO_REPORTADO) {
			//bufferDados_g.estadoBufferRecep = COD_ERRO;
			//bufferDados_g.estacomunicando = 'y';
			//bufferDados_g.dadosRecebidosComSucesso = 'n';
		//}
		//break;
		//
		//case INICIADO:
			//bufferDados_g.qntDadosAReceber = dadoRecebido;
			//if(bufferDados_g.qntDadosAReceber == 0)
				//bufferDados_g.estadoBufferRecep = IDLE;
			//bufferDados_g.estadoBufferRecep = RECEBENDO_DADOS;
		//break;
		//
		//case RECEBENDO_DADOS:
			//bufferRecepcao_g[contDados] = dadoRecebido;
			//contDados++;
		//if(contDados >= bufferDados_g.qntDadosAReceber) {
			//bufferDados_g.estadoBufferRecep = IDLE;
			//contDados = 0;
			//bufferDados_g.dadosRecebidosComSucesso = 'y';//provisorio ate implementar o checksum
		//}
		//break;
		//
		//case COD_ERRO:
			//bufferDados_g.erroCode = dadoRecebido;
			//bufferDados_g.estadoBufferRecep = IDLE;
		//break;
		//
		//case CHECKSUM:
		//case CONCLUIDO:
		//default:
			//bufferDados_g.estadoBufferRecep = IDLE;
	//}
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
	
	CarregaBufferTransmissao(&bufferDados_g, bufferTransmissao_g);		
	MostraDadosLCD(&bufferDados_g, bufferTransmissao_g);	
	TransmitiBuffer(bufferTransmissao_g);
		
	ADC_Init();	
	
	sei();
		
	while(1){				
	}
}


//----------------------------------------------------------------------------

void ValoresIniciaisBuffer()
{
	bufferDados_g.fonteAlimentacao = 'B';
	bufferDados_g.botaoSelFontePress = 'n';
	bufferDados_g.estacomunicando = 'n';
	bufferDados_g.dadosRecebidosComSucesso = 0;
	bufferDados_g.msgLCD4 = MSG_INICIAL;
}	

//----------------------------------------------------------------------------
