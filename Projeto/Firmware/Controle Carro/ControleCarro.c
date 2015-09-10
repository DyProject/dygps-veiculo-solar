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

unsigned char bufferRecepcao_g[QUANT_DADOS_PACOTE_TRANS];
//----------------------------------------------------------------------------

Buffer bufferRX_g;
	

//----------------------------------------------------------------------------

extern FILE stdoutUART;

void ValoresIniciaisBuffer();
void SolicitaReenvioDados();
void DescarregaDadosRecebidos();

//----------------------------------------------------------------------------

//char buff[10];

ISR(USART_RX_vect)							
{				
	stdout = &stdoutUART;
	
	char dadoRecebido = UDR0;	
	static uint8_t contDados = 0;
			
	switch(bufferRX_g.estadoBufferRecep) {
		case IDLE:
			if (dadoRecebido == START_TRANSMISSION) 
				bufferRX_g.estadoBufferRecep = INICIADO;
			bufferRX_g.dadosRecebidosComSucesso = 'n';
		break;
			
		case INICIADO:
			bufferRX_g.qntDadosAReceber = QUANT_DADOS_PACOTE_TRANS;//dadoRecebido;
			bufferRX_g.estadoBufferRecep = RECEBENDO_DADOS;
		break;
		
		case RECEBENDO_DADOS:		
			bufferRecepcao_g[contDados] = dadoRecebido;
			contDados++;
			if(contDados >= bufferRX_g.qntDadosAReceber) {
				bufferRX_g.estadoBufferRecep = IDLE;
				contDados = 0;
				bufferRX_g.dadosRecebidosComSucesso = 'y';//provisorio ate implementar o checksum
			}
				//bufferRX_g.estadoBufferRecep = CHECKSUM;
		break;
		
		case CHECKSUM:
		case CONCLUIDO:			
		default:
			bufferRX_g.estadoBufferRecep = IDLE;
			bufferRX_g.dadosRecebidosComSucesso = 'y';
	}

}	

//----------------------------------------------------------------------------

ISR(TIMER2_OVF_vect)
{					
	if (bufferRX_g.dadosRecebidosComSucesso == 'y') {
		DescarregaDadosRecebidos();
		
		DirecaoCarro(&bufferRX_g);
		AnguloServo(&bufferRX_g);
		TransmitiBuffer(&bufferRX_g);
		
		bufferRX_g.dadosRecebidosComSucesso = 'n';
		printf("z");
	}
}

//----------------------------------------------------------------------------
int main()
{
	Usart_Init(MYUBRR);
	//ADC_Init();
	ValoresIniciaisBuffer();
	ConfiguracoesDirecaoInit();
	
	/*Contador Timer 2*/
	TIMSK2 = 0b00000001;
	/*Prescaler do Timer2*/
	TCCR2B = (1<<CS22) | (1<<CS20);	
	
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
	bufferRX_g.dadosRecebidosComSucesso = 'n';
	bufferRX_g.estadoCarro = PARADO;
	bufferRX_g.estadoBufferRecep = IDLE;
	bufferRX_g.estadoBufferTrans = IDLE;
	bufferRX_g.erroCodeRecep = SEM_ERRO_RECEP;
}	

//----------------------------------------------------------------------------

void DescarregaDadosRecebidos()
{
	bufferRX_g.direcao = bufferRecepcao_g[DIRECAO];
	bufferRX_g.dutyCicleM1 = bufferRecepcao_g[DUTY_MOTOR_E];
	bufferRX_g.dutyCicleM2 = bufferRecepcao_g[DUTY_MOTOR_D];
	bufferRX_g.anguloServoLeft = bufferRecepcao_g[ANGULO_SERVO_E];
	bufferRX_g.anguloServoRight = bufferRecepcao_g[ANGULO_SERVO_D];
	bufferRX_g.fonteAlimentacao = bufferRecepcao_g[FONTE_ALIMENTACAO];
}

//----------------------------------------------------------------------------

