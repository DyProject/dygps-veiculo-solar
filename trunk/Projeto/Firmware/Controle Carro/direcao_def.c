#include "direcao_def.h"

#include "usart_def.h"

//----------------------------------------------------------------------------

#define AJUSTE_AD			0.0493
#define ATIVA_RELE()		(PORTB |= (1 << PB0))
#define DESATIVA_RELE()		(PORTB &= ~(1 << PB0))	

//----------------------------------------------------------------------------

volatile TEstadoCarro estadoCarro_g;
BufferRecep bufferRecepcao_g;

//----------------------------------------------------------------------------
	
/*Irá receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM1(
	uint8_t porCentagem
)
{
	uint16_t valor = 0;
	if(porCentagem == 100)
		valor = 35000;
	else if (porCentagem == 75)
		valor = 26250;
	else if (porCentagem == 50)
		valor = 17500;
	else if (porCentagem == 25)
		valor = 8750;
		
	if(estadoCarro_g == ANDANDO_TRAS)
		valor = 35000 - valor;
		
	return valor;
}

//----------------------------------------------------------------------------

/*Irá receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM2(
	uint8_t porCentagem
)
{
	uint16_t valor = 0;
	if (porCentagem == 100)
		valor = 35000;
	else if (porCentagem == 75)
		valor = 26250;
	else if (porCentagem == 50)
		valor = 17500;
	else if (porCentagem == 25)
		valor = 8750;
		
	if(estadoCarro_g == ANDANDO_TRAS)
		valor = 35000 - valor;
			
	return valor;
}

//----------------------------------------------------------------------------

void ConfiguracoesDirecaoInit()
{	
	DDRB|= (1 << IN2_IN4) | (1 << ENA_ENB);	
	PORTB |= (1 << IN2_IN4) | (1 << ENA_ENB);		
		
	/*pinos OC1B e OC1A como saída*/
	DDRB  |= (1 << IN1) | (1 << IN3);		
	PORTB &= (~(1 << IN1)) & (~(1 << IN3)); 
	
	/*Pino relé*/	
	DDRB |= (1 << PB0);
	DESATIVA_RELE();
	
	TCCR1A = 0b10100010;		//PWM não invertido nos pinos OC1A e OC1B
	TCCR1B = 0b00011001;		//liga TC1, prescaler = 1
	ICR1 = 35000;				//valor máximo para contagem
	OCR1A = 0;				//controle do ciclo ativo do PWM 0C1A
	OCR1B = 0;
	
	/*Configurações de inicialização d do buffer*/
	bufferRecepcao_g.qntdDadosLido = 0;
	bufferRecepcao_g.iniciado = 0;
	
	//Prescaler do Timer0, usado para fazer uma leitura do ADC.
	TCCR0B = (1<<CS02) | (1<<CS00);
					
	ADC_Init();
	
	CarroParado();
};

//----------------------------------------------------------------------------

void DirecaoCarro()
{		
	switch (estadoCarro_g) {
		case PARADO:
			if ( bufferRecepcao_g.direcao == 'F') {
				estadoCarro_g = ANDANDO_FRENTE;
				AndandoFrente();
			}				
			else if ( bufferRecepcao_g.direcao == 'T') {
				estadoCarro_g = ANDANDO_TRAS;
				AndandoTras();		
			}			
			else bufferRecepcao_g.direcao = 'P';
			break;
		
		case ANDANDO_FRENTE:	
			if(bufferRecepcao_g.direcao == 'P') {
				estadoCarro_g = PARADO;
				CarroParado();
			}				
			else if(bufferRecepcao_g.direcao == 'F'){
				OCR1A = CalculaDutyCicleM1(bufferRecepcao_g.dutyCicleM1);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao_g.dutyCicleM2);
			}			
			break;
			
		case ANDANDO_TRAS:
			if(bufferRecepcao_g.direcao == 'P') {
				estadoCarro_g = PARADO;
				CarroParado();
			}				
			else if(bufferRecepcao_g.direcao == 'T'){
				OCR1A = CalculaDutyCicleM1(bufferRecepcao_g.dutyCicleM1);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao_g.dutyCicleM2);
			}			
			break;			
	}		
}		

//----------------------------------------------------------------------------

void AndandoFrente()
{
	_delay_us(10);
	set_bit(PORTB,ENA_ENB);	
	clr_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(bufferRecepcao_g.dutyCicleM1);
	OCR1B = CalculaDutyCicleM2(bufferRecepcao_g.dutyCicleM2);
	estadoCarro_g = ANDANDO_FRENTE;
}

//----------------------------------------------------------------------------

void AndandoTras()
{
	_delay_us(10);
	set_bit(PORTB,ENA_ENB);	
	set_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(bufferRecepcao_g.dutyCicleM1);
	OCR1B = CalculaDutyCicleM2(bufferRecepcao_g.dutyCicleM2);
	estadoCarro_g = ANDANDO_TRAS;
}

//----------------------------------------------------------------------------

void CarroParado()
{
	_delay_us(10);
	clr_bit(PORTB,ENA_ENB);	
	set_bit(PORTB,IN1);	
	set_bit(PORTB,IN3);	
	set_bit(PORTB,IN2_IN4);	
	estadoCarro_g = PARADO;
}

//----------------------------------------------------------------------------

void RecebeProtocolo()
{	
	uint16_t valorLidoAD = UDR0;
	if(bufferRecepcao_g.iniciado) {
		bufferRecepcao_g.qntdDadosLido++;
		
		switch(bufferRecepcao_g.qntdDadosLido){
		case 1:
			bufferRecepcao_g.direcao = valorLidoAD;
			break;
		case 2:
			bufferRecepcao_g.dutyCicleM1 = valorLidoAD;
			break;
		case 3:
			bufferRecepcao_g.dutyCicleM2 = valorLidoAD;
			bufferRecepcao_g.iniciado = 0;
			bufferRecepcao_g.qntdDadosLido = 0;
			
			DirecaoCarro(bufferRecepcao_g);
			
			/*Indica recebimento do protocolo*/
			Usart_Transmit('z');
	
			break;
		}
	}		
	
	/*Inicia transmissão*/
	if(valorLidoAD == 'S')
		bufferRecepcao_g.iniciado = 1;			
}	

//----------------------------------------------------------------------------