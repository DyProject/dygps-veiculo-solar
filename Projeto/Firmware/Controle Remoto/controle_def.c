#include "controle_def.h"

#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;
unsigned char caracterConvertido_g[5];

//----------------------------------------------------------------------------

void TransmitiBuffer(
	uint8_t dutyLadoEsq, 
	uint8_t dutyLadoDir, 
	unsigned char direcao
)
{
	unsigned char buffer[5];
	
	buffer[0] = 'S';	
	buffer[1] = direcao;
	buffer[2] = dutyLadoEsq;
	buffer[3] = dutyLadoDir;
	buffer[4] = '\0';
	
	Usart_Transmit(buffer[0]);
	Usart_Transmit(buffer[1]);
	Usart_Transmit(buffer[2]);
	Usart_Transmit(buffer[3]);
	
	//Usart_Write(buffer);
}

//----------------------------------------------------------------------------

void Inicializacoes()
{
	Usart_Init(MYUBRR);
	
	//Prescaler do Timer0, usado para fazer uma leitura do ADC.
	TCCR0B = (1<<CS02) | (1<<CS00);
	
	
	inic_LCD_4bits();					
	ADC_Init();
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
	CvrtNum2CaracterIndividual(dutyLadoEsq, caracConvertido, 5);
	escreve_LCD(caracConvertido);
	
	CvrtNum2CaracterIndividual(dutyLadoDir, caracConvertido, 5);
	escreve_LCD(caracConvertido);
	
	EscreveCaracterLCD(']');
}

//----------------------------------------------------------------------------

void Protocolo()
{
	unsigned char direcao;
	unsigned char sentido;		
	uint8_t dutyLadorEsq;
	uint8_t dutyLadorDir;
		
	dutyLadorEsq = CalculaDutyCycleLadoEsq();
	dutyLadorDir= CalculaDutyCycleLadoDir();
		
	sentido = CalculaSentido();
	direcao = DirecaoCarro(sentido);
		
	TransmitiBuffer(dutyLadorEsq, dutyLadorDir, direcao);
	MostraDadosLCD(dutyLadorEsq, dutyLadorDir, sentido);
}	

//----------------------------------------------------------------------------