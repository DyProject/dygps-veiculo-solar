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
#include <util/delay.h>
#include <stdint.h>

#include "usart_def.h"

//----------------------------------------------------------------------------

#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))

#define IN1			PB1
#define IN3			PB2
#define	IN2_IN4		PB3
#define ENA			PB4
#define ENB			PB5

//----------------------------------------------------------------------------
 
 typedef enum {
	PARADO = 1, ANDANDO_FRENTE, ANDANDO_TRAS, ANDANDO_FRENTE_DIREITA, ANDANDO_FRENTE_ESQUERDA,
	ANDANDO_TRAS_DIREITA, ANDANDO_TRAS_ESQUERDA 
	}TEstadoCarro;
	
volatile TEstadoCarro estadoCarro_g = PARADO;
uint8_t direcaoCarroRecebido_g, dutyCicleM1Recebido_g, dutyCicleM2Recebido_g;

struct{
	uint8_t direcao;
	uint8_t dutyCicleM1;
	uint8_t dutyCicleM2;
	volatile uint8_t completo;
	volatile uint8_t lido;
	uint8_t qntdDadosLido;
}bufferRecepcao_g;

//----------------------------------------------------------------------------

void ConfiguraPWM();

void HabilitaMotor(
	uint8_t motor
);

void DesabilitaMotor(
	uint8_t motor
);

uint16_t CalculaDutyCicleM1(
	uint8_t porCentagem
);

uint16_t CalculaDutyCicleM2(
	uint8_t porCentagem
);

void DirecaoCarro();

void CarroParado();
void AndandoFrente();
void AndandoFrenteDireita();
void AndandoFrenteEsquerda();
void AndandoTras();
void AndandoTrasDireita();
void AndandoTrasEsquerda();

void BufferRecepcaoInit();
//----------------------------------------------------------------------------

ISR(USART_RX_vect)							
{
	unsigned char dummy;
	bufferRecepcao_g.qntdDadosLido++;
	bufferRecepcao_g.completo = 0;
	switch(bufferRecepcao_g.qntdDadosLido){
		case 1:
			bufferRecepcao_g.direcao = UDR0;
			break;
		case 2:
			bufferRecepcao_g.dutyCicleM1 = UDR0;
			break;
		case 3:
			bufferRecepcao_g.dutyCicleM2 = UDR0;
			bufferRecepcao_g.completo = 1;
			bufferRecepcao_g.lido = 0;
			bufferRecepcao_g.qntdDadosLido = 0;
			
			direcaoCarroRecebido_g = bufferRecepcao_g.direcao;
			dutyCicleM1Recebido_g = bufferRecepcao_g.dutyCicleM1;
			dutyCicleM2Recebido_g = bufferRecepcao_g.dutyCicleM2;
			
			/*Indica Fim da transmissão*/
			Usart_Transmit('z');
			
			break;
		default:
			dummy = UDR0;
	}		
}	

//----------------------------------------------------------------------------

int main()
{
    DDRB	|= (1 << IN2_IN4) | (1 << ENA) | (1 << ENB) | (1 << IN1) | (1 << IN3);	
	PORTB |= (1 << IN2_IN4) | (1 << ENA) | (1 << ENB);		
		
	ConfiguraPWM();
	CarroParado();
	Usart_Init(MYUBRR);
	BufferRecepcaoInit();
	
	sei();
			
	while(1)
	{	
		if(bufferRecepcao_g.completo && (!bufferRecepcao_g.lido)) {
			DirecaoCarro();
			bufferRecepcao_g.lido = 1;
			Usart_Transmit('[');
			Usart_Transmit(direcaoCarroRecebido_g);
			Usart_Transmit(dutyCicleM1Recebido_g);
			Usart_Transmit(dutyCicleM2Recebido_g);
			Usart_Transmit(']');	
			
		}			
	}		
} /*MAIN*/

//----------------------------------------------------------------------------

void BufferRecepcaoInit(){
	bufferRecepcao_g.qntdDadosLido = 0;
	bufferRecepcao_g.completo = 0;
	bufferRecepcao_g.lido = 0;
}	
	
//----------------------------------------------------------------------------
	

uint16_t CalculaDutyCicleM1(
	uint8_t porCentagem
)
{
	return ICR1 * porCentagem / 100;
}

//----------------------------------------------------------------------------

uint16_t CalculaDutyCicleM2(
	uint8_t porCentagem
)
{
	return ICR1 * porCentagem / 100;
}

//----------------------------------------------------------------------------

void ConfiguraPWM()
{
	/*pinos OC1B e OC1A como saída*/
	DDRB  |= (1 << PB1) | (1 << PB2);		
	PORTB &= (~(1 << PB1)) & (~(1 << PB2)); 
	
	TCCR1A = 0b10100010;		//PWM não invertido nos pinos OC1A e OC1B
	TCCR1B = 0b00011001;		//liga TC1, prescaler = 1
	ICR1 = 35000;				//valor máximo para contagem
	OCR1A = 0;				//controle do ciclo ativo do PWM 0C1A
	OCR1B = 0;
};

//----------------------------------------------------------------------------
void DirecaoCarro()
{	
	switch (estadoCarro_g) {
		case PARADO:
			if ( direcaoCarroRecebido_g == 'F')
				AndandoFrente();
			else if ( direcaoCarroRecebido_g == 'R')
				AndandoTras();		
			break;
			
		case ANDANDO_FRENTE:
			if(direcaoCarroRecebido_g == 'D')
				AndandoFrenteDireita();
			else if(direcaoCarroRecebido_g == 'E') 
				AndandoFrenteEsquerda();	
			else if(direcaoCarroRecebido_g == 'P')
				CarroParado();
			else {
				OCR1A = CalculaDutyCicleM1(dutyCicleM1Recebido_g);
				OCR1B = CalculaDutyCicleM2(dutyCicleM2Recebido_g);
			};				
			break;
			
		case ANDANDO_FRENTE_DIREITA:
			if(direcaoCarroRecebido_g == 'E')
				AndandoFrenteEsquerda();
			else if(direcaoCarroRecebido_g == 'F') 
				AndandoFrente();	
			else if(direcaoCarroRecebido_g == 'P')
				CarroParado();			
			break;
		
		case ANDANDO_FRENTE_ESQUERDA:
			if(direcaoCarroRecebido_g == 'D')
				AndandoFrenteDireita();
			else if(direcaoCarroRecebido_g == 'F') 
				AndandoFrente();	
			else if(direcaoCarroRecebido_g == 'P')
				CarroParado();			
			break;
						
		case ANDANDO_TRAS:
			if(direcaoCarroRecebido_g == 'D') 
				AndandoTrasDireita();
			else if(direcaoCarroRecebido_g == 'E')
				AndandoTrasEsquerda();
			else if(direcaoCarroRecebido_g == 'P')	
				CarroParado();	
			break;
			
		case ANDANDO_TRAS_DIREITA:
			if(direcaoCarroRecebido_g == 'E')
				AndandoTrasEsquerda();
			else if(direcaoCarroRecebido_g == 'R')
				AndandoTras();
			else if(direcaoCarroRecebido_g == 'P') 
				CarroParado();
			break;
			
		case ANDANDO_TRAS_ESQUERDA:
			if(direcaoCarroRecebido_g == 'D')
				AndandoTrasDireita();
			else if(direcaoCarroRecebido_g == 'R')
				AndandoTras();
			else if(direcaoCarroRecebido_g == 'P') 
				CarroParado();
			break;						
	}
				
}		

//----------------------------------------------------------------------------

void AndandoFrente()
{
	_delay_us(10);
	clr_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(dutyCicleM1Recebido_g);
	OCR1B = CalculaDutyCicleM2(dutyCicleM2Recebido_g);
	HabilitaMotor(1);
	HabilitaMotor(2);
	estadoCarro_g = ANDANDO_FRENTE;
	Usart_Transmit('F');
}

//----------------------------------------------------------------------------

void AndandoTras()
{
	_delay_us(10);
	set_bit(PORTB,IN2_IN4);
	OCR1A = 0;
	OCR1B = 0;
	HabilitaMotor(1);
	HabilitaMotor(2);
	estadoCarro_g = ANDANDO_TRAS;
	Usart_Transmit('R');
}
//----------------------------------------------------------------------------

void AndandoFrenteDireita()
{
	_delay_us(10);
	clr_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(dutyCicleM1Recebido_g);
	OCR1B = CalculaDutyCicleM2(dutyCicleM2Recebido_g);
	HabilitaMotor(1);
	DesabilitaMotor(2);
	estadoCarro_g = ANDANDO_FRENTE_DIREITA;
	Usart_Transmit('F');
	Usart_Transmit('D');
}
//----------------------------------------------------------------------------

void AndandoFrenteEsquerda()
{
	_delay_us(10);
	clr_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(dutyCicleM1Recebido_g);
	OCR1B = CalculaDutyCicleM2(dutyCicleM2Recebido_g);
	HabilitaMotor(2);
	DesabilitaMotor(1);
	estadoCarro_g = ANDANDO_FRENTE_ESQUERDA;
	Usart_Transmit('F');
	Usart_Transmit('E');
}
//----------------------------------------------------------------------------

void AndandoTrasDireita()
{
	_delay_us(10);
	set_bit(PORTB,IN2_IN4);
	OCR1A = 0;
	HabilitaMotor(1);
	DesabilitaMotor(2);
	estadoCarro_g = ANDANDO_TRAS_DIREITA;
	Usart_Transmit('R');
	Usart_Transmit('D');
}
//----------------------------------------------------------------------------

void AndandoTrasEsquerda()
{
	_delay_us(10);
	set_bit(PORTB,IN2_IN4);
	OCR1B = 0;
	HabilitaMotor(2);
	DesabilitaMotor(1);
	estadoCarro_g = ANDANDO_TRAS_ESQUERDA;
	Usart_Transmit('T');
	Usart_Transmit('E');
}
//----------------------------------------------------------------------------

void CarroParado()
{
	DesabilitaMotor(1);
	DesabilitaMotor(2);
	estadoCarro_g = PARADO;
	Usart_Transmit('P');
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void HabilitaMotor(
	uint8_t motor
)
{
	if (motor == 1)
		set_bit(PORTB, ENA);
	else if (motor == 2)
		set_bit(PORTB, ENB);
}

//----------------------------------------------------------------------------

void DesabilitaMotor(
	uint8_t motor
)
{
	if (motor == 1)
		clr_bit(PORTB, ENA);
	if (motor == 2)
		clr_bit(PORTB, ENB);
}

//----------------------------------------------------------------------------